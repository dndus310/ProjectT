// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldGameMode.h"


#include "CorePlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/DataTableFunctionLibrary.h"
#include "ProjectT/Data/Gen/GenerateEnumDataTableKey.h"
#include "ProjectT/Data/Gen/GenerateEnumStage.h"
#include "ProjectT/System/Core/Characters/NPC/CoreNpc.h"
#include "ProjectT/System/Core/Characters/Skill/NpcRAtk.h"
#include "ProjectT/System/Core/Characters/Skill/NpcGAtk.h"
#include "ProjectT/System/Core/Common/AssetUtilsLibrary.h"
#include "ProjectT/System/Core/Interfaces/ProcessStateControl.h"
#include "ProjectT/System/Core/Managers/StageManager.h"
#include "ProjectT/System/Core/Managers/SequenceManager.h"
#include "ProjectT/System/Core/Common/GlobalUtilsLibrary.h"
#include "ProjectT/System/Core/Interfaces/GlobalObjectProvider.h"
#include "ProjectT/System/Core/Managers/CoreCheatManager.h"
#include "ProjectT/System/Core/Managers/DataDam.h"
#include "ProjectT/System/Core/Managers/OptionManager.h"
#include "ProjectT/System/Core/Managers/SoundManager.h"
#include "ProjectT/System/Core/Managers/NotificationCenter.h"
#include "ProjectT/System/Core/Managers/PooledActorManager.h"
#include "ProjectT/System/Core/Managers/QuestManager.h"
#include "ProjectT/System/Core/Objects/LevelObject/ItemObject.h"
#include "ProjectT/System/Core/SaveGame/UserSaveGame.h"
#include "ProjectT/System/SinglePlay/Characters/Player/SPPlayerCharacter.h"

AWorldGameMode::AWorldGameMode() :
	SequenceManager(nullptr)
{
}

void AWorldGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

USequenceManager* AWorldGameMode::GetSequenceManager_Implementation()
{
	return SequenceManager;
}

UPooledActorManager* AWorldGameMode::GetPooledActorManager_Implementation()
{
	return PooledActorManager;
}

UQuestManager* AWorldGameMode::GetQuestManager_Implementation()
{
	return QuestManager;
}

UCoreWidgetManager* AWorldGameMode::GetCoreWidgetManager_Implementation()
{
	return Super::GetCoreWidgetManager_Implementation();
}

void AWorldGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);
}

void AWorldGameMode::BeginPlay()
{
	Super::BeginPlay();
	InitializeManagers();
	InitializePlayerSpawnPoints();
}

void AWorldGameMode::BeginDestroy()
{
	Super::BeginDestroy();
}

void AWorldGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	UGameInstance* GI = GetGameInstance();
	NMT_CHECKF(GI);

	UNotificationCenter* NC = IGlobalObjectProvider::Execute_GetNotificationCenter(GI);
	NC->ReleaseNotificationCenter();
	
	USoundManager* SoundMgr = IGlobalObjectProvider::Execute_GetSoundManager(GI);
	SoundMgr->ReleaseManager();
	
	UOptionManager* OptionMgr = IGlobalObjectProvider::Execute_GetOptionManager(GI);
	OptionMgr->ReleaseManager();

	UStageManager* StageMgr = IGlobalObjectProvider::Execute_GetStageManager(GI);
	StageMgr->ReleaseManager();

	UCoreCheatManager* CheatMgr = IGlobalObjectProvider::Execute_GetCoreCheatManager(GI);
	CheatMgr->ReleaseManager();

	SequenceManager->ReleaseManager();
	SequenceManager = nullptr;
	
	QuestManager->ReleaseManager();
	QuestManager = nullptr;

	PooledActorManager->ReleaseManager();
	PooledActorManager = nullptr;

	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
}

void AWorldGameMode::InitializeManagers()
{
	Super::InitializeManagers();
	
	UWorld* World = GetWorld();
	NMT_CHECKF(World);
	
	if (!SequenceManager)
	{
		SequenceManager = NewObject<USequenceManager>(this);
		SequenceManager->InitializeSequenceManager(World);
		NMT_CHECKF(SequenceManager);
	}

	if (!PooledActorManager)
	{
		PooledActorManager = NewObject<UPooledActorManager>(this);
		NMT_CHECKF(PooledActorManager);
	}

	if(!QuestManager)
	{
		QuestManager = NewObject<UQuestManager>(this);
		QuestManager->InitializeManager(this);
		NMT_CHECKF(QuestManager);
	}
}

void AWorldGameMode::InitializeNotification()
{
    NOT_IMPLEMENTED()
}

void AWorldGameMode::InitializePlayerSpawnPoints()
{
	UWorld* World = UGlobalUtilsLibrary::GetValidWorld(this);
	
	UGameplayStatics::GetAllActorsOfClass(World, APlayerStart::StaticClass(), PlayerSpawnPoints);
	if(PlayerSpawnPoints.IsEmpty())
	{
		AActor* Temp = World->SpawnActor<AActor>(AActor::StaticClass(), FVector(), FRotator());
		PlayerSpawnPoints.Push(Temp);
		NMT_MSG_ENSURE(0, "Player Spawn Points is Empty");
	}
}

void AWorldGameMode::InitializePlayer()
{
	UWorld* World = UGlobalUtilsLibrary::GetValidWorld(this);
	UGlobalUtilsLibrary::ShuffleArray(PlayerSpawnPoints);

	UUserSaveGame* USG = nullptr;
	UUserSaveGame::LoadUserSaveGame(USG);

	FString SCD;
	USG->GetSelectedCharacterData(SCD);
	if(SCD.IsEmpty())
	{
		TArray<FUnlockCharacterData> UCD;
		USG->GetUnlockCharacterData(UCD);
		SCD = UCD[0].UnlockCharacterRowName;
		USG->SetCurrentSelectedCharacterData(SCD);
	}

	FString FoundBlueprintPath;
	UDataDam* DD = IGlobalObjectProvider::Execute_GetDataDam(GetGameInstance());
	const UDataTable* FoundTable = DD->GetDataTableByEnum(EDataTableKey::PlayerCharacter);
	TMap<FName, uint8*> RowMap = FoundTable->GetRowMap();
	for(TTuple<FName, unsigned char*> Pair : RowMap)
	{
		if(Pair.Key.IsEqual(*SCD))
		{
			FPlayerCharacter* FoundData = reinterpret_cast<FPlayerCharacter*>(Pair.Value);
			FoundBlueprintPath = FoundData->sCharBlueprintPath;
			break;
		}
	}

	FString ClassPath = UAssetUtilsLibrary::AssetReferencePathToClassPath(FoundBlueprintPath);
	UClass* LoadedClass = StaticLoadClass(UObject::StaticClass(), nullptr, *ClassPath);
	NMT_CHECKF(LoadedClass);
	
	FVector Location = PlayerSpawnPoints[0]->GetActorLocation();
	FRotator Rotator = PlayerSpawnPoints[0]->GetActorRotation();
	AActor* SpawnActor = World->SpawnActor(LoadedClass, &Location, &Rotator);

	ACorePlayerCharacter* Player = Cast<ACorePlayerCharacter>(SpawnActor);
	NMT_MSG_CHECKF(Player, "%s: Character Class Not Found PlayerCharacter.csv -> RowName[%s] ", *LoadedClass->GetName(), *SCD)
	
	ACorePlayerController* CPC = UGlobalUtilsLibrary::GetLocalPlayerControllerChecked<ACorePlayerController>(Player);
	CPC->Possess(Player);

	TSubclassOf<AActor> LocAtkSkillClass;
	TSubclassOf<AActor> LocSkillClass;
	Player->GetSkillClass(LocAtkSkillClass, LocSkillClass);
	
	const int32 LocAtkPoolSize = TSubclassOf<ACoreSkill>(LocAtkSkillClass).GetDefaultObject()->GetSkillPoolSize();
	const int32 LocSkillPoolSize = TSubclassOf<ACoreSkill>(LocSkillClass).GetDefaultObject()->GetSkillPoolSize();
	
	PooledActorManager->CreatePool(LocAtkSkillClass, LocAtkPoolSize);
	PooledActorManager->CreatePool(LocSkillClass, LocSkillPoolSize);
}

void AWorldGameMode::InitializeUI()
{
	NOT_IMPLEMENTED();
}

void AWorldGameMode::InitializeItemObject()
{
	UDataTable* FoundTable = UAssetUtilsLibrary::LoadDataTableByEnumKey(EDataTableKey::Skill);
	TArray<AActor*> LocItemArray;
	TArray<int> LocItemSkillIds;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AItemObject::StaticClass(),LocItemArray);
	for(AActor* LocItem : LocItemArray)
	{
		AItemObject* LocCastItem = Cast<AItemObject>(LocItem);
		if(LocCastItem)
		{
			if(LocCastItem->GetItemData().iItemSkillId != 0)
				LocItemSkillIds.AddUnique(LocCastItem->GetItemData().iItemSkillId);
		}
	}
	for(int LocSkillId : LocItemSkillIds)
	{
		FString LocSkillRow = FString::FromInt(LocSkillId);
		FSkill* SkillDataRow = FoundTable->FindRow<FSkill>(*LocSkillRow, TEXT("Not Found FSkill"));
		FString TempPath = SkillDataRow->sSkillBpPath;
		if (!CHECK_FREE(TempPath))
		{
			FString ClassPath = UAssetUtilsLibrary::AssetReferencePathToClassPath(TempPath);
			UClass* LoadedClass = StaticLoadClass(UObject::StaticClass(), nullptr, *ClassPath);
			NMT_CHECKF(LoadedClass);
			
			PooledActorManager->CreatePool(LoadedClass, 1);
		}
	}
}

void AWorldGameMode::InitializeEnemyCharacters()
{
	UWorld* World = UGlobalUtilsLibrary::GetValidWorld(this);
	UDataTable* SkillTable = UAssetUtilsLibrary::LoadDataTableByEnumKey(EDataTableKey::Skill);
	NMT_CHECKF(SkillTable);
		
	UGameplayStatics::GetAllActorsOfClass(World, ACoreNpc::StaticClass(), NpcCharacters);
	if(NpcCharacters.IsEmpty())	NMT_MSG_ENSURE(0, "Npc Character is Empty");

	TMap<FString, TTuple<FString, int32>> SKillMap;
	FSkill* SkillData = nullptr;
	for(AActor* Enemy : NpcCharacters)
	{
		ACoreNpc* CN = CastChecked<ACoreNpc>(Enemy);
		uint8* FoundData = SkillTable->FindRowUnchecked(FROM_INT(CN->GetCurrentNpcStatus().iNpcAttackId));
		if(!FoundData) continue;

		SkillData = reinterpret_cast<FSkill*>(FoundData);
		NMT_CHECKF(SkillData);

		TPair<FString, signed int>& SkillCount = SKillMap.FindOrAdd(SkillData->sSkillName);
		SkillCount.Key = SkillData->sSkillBpPath;
		SkillCount.Value += 1;
	}

	for(TTuple<FString, TTuple<FString, int32>> Item : SKillMap)
	{
		FString SkilClassPath = Item.Value.Key;
		if(!CHECK_FREE(SkilClassPath))
		{
			FString ClassPath = UAssetUtilsLibrary::AssetReferencePathToClassPath(SkilClassPath);
			UClass* LoadedClass = StaticLoadClass(UObject::StaticClass(), nullptr, *ClassPath);
			NMT_CHECKF(LoadedClass);
		
			TSubclassOf<AActor> NpcAtkSkillClass = TSubclassOf<AActor>(LoadedClass);
			PooledActorManager->CreatePool(NpcAtkSkillClass, Item.Value.Value * 1);
		}	
	}
}

void AWorldGameMode::PauseGame_Implementation()
{
	UWorld* World = GetWorld();
	NMT_CHECKF(World);
	
	if(World->IsPaused())
		return;
	
	UGameplayStatics::SetGamePaused(World, true);	
}

void AWorldGameMode::UnPauseGame_Implementation()
{
	UWorld* World = GetWorld();
	NMT_CHECKF(World);

	if(!World->IsPaused())
		return;

	UGameplayStatics::SetGamePaused(World, false);
}

bool AWorldGameMode::ChangedProcessEvent(const EProcessState ProcessState)
{
	UWorld* World = UGlobalUtilsLibrary::GetValidWorld(this);
	switch (ProcessState)
	{
		case EProcessState::Ready:
		{
			InitializeNotification();
			break;
		}
		case EProcessState::Intro: break;
		case EProcessState::PreGame: break;
		case EProcessState::GamePlaying: break;
		case EProcessState::Victory: break;
		case EProcessState::Defeat: break;
		case EProcessState::Reward:
		{
			break;
		}
		case EProcessState::Finish: break;
		case EProcessState::None: default: NMT_CHECKF(0);
	}
	return true;
}

void AWorldGameMode::GetPlayerSpawnPoints(TArray<AActor*>& OutSpawnPoints) const
{
	OutSpawnPoints = PlayerSpawnPoints;
}

ACorePlayerCharacter* AWorldGameMode::GetPlayerCharacter()
{
	UWorld* World = UGlobalUtilsLibrary::GetValidWorld(this);
	
	ACorePlayerCharacter* Ret = UGlobalUtilsLibrary::GetLocalPlayerUnChecked<ACorePlayerCharacter>(World);
	if(!Ret)
	{
		AActor* FoundActor = UGameplayStatics::GetActorOfClass(World, ACorePlayerCharacter::StaticClass());
		NMT_CHECKF(FoundActor);
		Ret = CastChecked<ACorePlayerCharacter>(FoundActor);
	}
	
	return Ret;
}
