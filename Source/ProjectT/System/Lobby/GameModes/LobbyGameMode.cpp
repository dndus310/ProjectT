// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"
#include "ProjectT/ProjectT.h"
#include "ProjectT/System/Lobby/Managers/LobbyWidgetManager.h"
#include "ProjectT/System/Core/Common/GlobalUtilsLibrary.h"
#include "ProjectT/System/Core/Interfaces/GlobalObjectProvider.h"
#include "ProjectT/System/Core/Managers/DataDam.h"
#include "ProjectT/Data/Gen/GenerateEnumDataTableKey.h"
#include "ProjectT/System/Lobby/Objects/LobbyPreviewCharacter.h"


ALobbyGameMode::ALobbyGameMode()
	:CurrentStreamLevel(nullptr),
	 StageIDX(0)
{

}

void ALobbyGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);	
}

ULobbyWidgetManager* ALobbyGameMode::GetLobbyWidgetManager_Implementation()
{
	NMT_MSG_CHECKF(CoreWidgetManager, "Widget Manager is Null");
	ULobbyWidgetManager* LWM = Cast<ULobbyWidgetManager>(CoreWidgetManager);
	NMT_MSG_CHECKF(LWM, "LobbyWidgetManager Cast Failed");
	return LWM;
}

ALobbyPreviewCharacter* ALobbyGameMode::GetLobbyPreviewCharacter_Implementation()
{
	AActor* FindActor;
	FindActor = UGameplayStatics::GetActorOfClass(GetWorld(), ALobbyPreviewCharacter::StaticClass());
	NMT_MSG_CHECKF(FindActor, "LobbyPreviewCharacter not found");
	ALobbyPreviewCharacter* LPC = Cast<ALobbyPreviewCharacter>(FindActor); 
	NMT_MSG_CHECKF(LPC, "Cast fail to LobbyPreviewCharacter");

	return LPC;
}


void ALobbyGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);	
	InitializeManagers();
	InitializeUI();
	CurrentStreamLevel = LobbyLevels[0];
}

void ALobbyGameMode::BeginPlay()
{
	Super::BeginPlay();	
}

void ALobbyGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void ALobbyGameMode::InitializeManagers()
{
	Super::InitializeManagers();	
	UWorld* World = GetWorld();
	NMT_CHECKF(World);

	if(!CoreWidgetManager)
	{
		CoreWidgetManager = NewObject<ULobbyWidgetManager>(this);
		CoreWidgetManager->InitializeManager(this);
		NMT_CHECKF(CoreWidgetManager);
	}
}

void ALobbyGameMode::InitializeUI()
{
	Super::InitializeUI();
	UWorld* World = UGlobalUtilsLibrary::GetValidWorld(this);
	
	NMT_CHECKF(CoreWidgetManager);
	ULobbyWidgetManager* LWM = CastChecked<ULobbyWidgetManager>(CoreWidgetManager);
	LWM->OnRequestLevelChange.BindUFunction(this, FName("LevelChange"));
	UDataDam* DD = IGlobalObjectProvider::Execute_GetDataDam(GetGameInstance());
	NMT_CHECKF(DD);

	const UDataTable* StageTable = DD->GetDataTableByEnum(EDataTableKey::Stage);
	NMT_CHECKF(StageTable);
	for(FName StageRow : StageTable->GetRowNames())
	{
		FStage* StageData = StageTable->FindRow<FStage>(StageRow, TEXT("Not Found StageDataRow"));
		if(StageData)
		{			
			LWM->StageDatas.Add(StageRow, *StageData);
		}		
	}	

	const UDataTable* PlayerCharacterTable = DD->GetDataTableByEnum(EDataTableKey::PlayerCharacter);
	NMT_CHECKF(PlayerCharacterTable);
	for(FName PlayerCharacterRow : PlayerCharacterTable->GetRowNames())
	{
		FPlayerCharacter* PlayerCharacterData = PlayerCharacterTable->FindRow<FPlayerCharacter>(PlayerCharacterRow, TEXT("Not Found PlayerCharacterDataRow"));
		if(PlayerCharacterData)
		{
			LWM->PlayerCharacterDatas.Add(PlayerCharacterRow, *PlayerCharacterData);
		}
	}

	const UDataTable* SkillTable = DD->GetDataTableByEnum(EDataTableKey::Skill);
	NMT_CHECKF(SkillTable);
	for(FName SkillRow : SkillTable->GetRowNames())
	{
		FSkill* SkillData = SkillTable->FindRow<FSkill>(SkillRow, TEXT("Not Found SkillDataRow"));
		if(SkillData)
		{
			LWM->SkillDatas.Add(SkillRow, *SkillData);
		}
	}

	const UDataTable* ObjectiveTable = DD->GetDataTableByEnum(EDataTableKey::Objective);
	NMT_CHECKF(ObjectiveTable);
	for(FName ObjectiveRow : ObjectiveTable->GetRowNames())
	{
		FObjective* ObjectiveData = ObjectiveTable->FindRow<FObjective>(ObjectiveRow, TEXT("Not Found ObjectiveDataRow"));
		if(ObjectiveData)
		{
			LWM->ObjectiveDatas.Add(ObjectiveRow, *ObjectiveData);
		}
	}	
}

void ALobbyGameMode::LevelChange(int32 LobbyLevelIDX)
{
	StageIDX = LobbyLevelIDX;
	if(LobbyLevels.IsValidIndex(StageIDX))
	{
		FLatentActionInfo LatentInfo;
		LatentInfo.CallbackTarget = this;
		LatentInfo.ExecutionFunction = FName("CompleteLevelUnLoad");
		LatentInfo.UUID = 1;
		LatentInfo.Linkage = 0;				
				
		UGameplayStatics::UnloadStreamLevelBySoftObjectPtr(
			this,
			CurrentStreamLevel,			
			LatentInfo,
			true
		);
	}

}

void ALobbyGameMode::CompleteLevelLoad()
{
	ULobbyWidgetManager* LWM = CastChecked<ULobbyWidgetManager>(CoreWidgetManager);
	CurrentStreamLevel = LobbyLevels[StageIDX];
	LWM->OnCompleteLevelChange.ExecuteIfBound(StageIDX);
}

void ALobbyGameMode::CompleteLevelUnLoad()
{
	if(LobbyLevels.IsValidIndex(StageIDX))
	{
		FLatentActionInfo LatentInfo;
		LatentInfo.CallbackTarget = this;
		LatentInfo.ExecutionFunction = FName("CompleteLevelLoad");
		LatentInfo.UUID = 2;
		LatentInfo.Linkage = 0;
		const TSoftObjectPtr<UWorld>& LevelToLoad = LobbyLevels[StageIDX];

		UGameplayStatics::LoadStreamLevelBySoftObjectPtr(
			this,
			LevelToLoad,
			true,
			true,
			LatentInfo
		);
	}
}

