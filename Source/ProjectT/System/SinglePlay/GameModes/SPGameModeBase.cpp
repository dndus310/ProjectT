// Fill out your copyright notice in the Description page of Project Settings.


#include "SPGameModeBase.h"

#include "Kismet/KismetMathLibrary.h"
#include "ProjectT/ProjectT.h"
#include "ProjectT/Data/Gen/GenerateEnumDataTableKey.h"
#include "ProjectT/Data/Gen/GenerateEnumObjectiveTypes.h"
#include "ProjectT/Data/Gen/GenerateEnumStage.h"
#include "ProjectT/Data/Gen/GenerateStructCharacterUnlock.h"
#include "ProjectT/Data/Paths/BaseDataPathLibrary.h"
#include "ProjectT/System/Core/Characters/NPC/CoreNpc.h"
#include "ProjectT/System/Core/Common/GlobalUtilsLibrary.h"
#include "ProjectT/System/Core/Components/PlayerStatusComponent.h"
#include "ProjectT/System/Core/GameModes/CoreGameState.h"
#include "ProjectT/System/Core/GameModes/CorePlayerController.h"
#include "ProjectT/System/Core/Interfaces/GlobalObjectProvider.h"
#include "ProjectT/System/Core/Interfaces/ProcessStateControl.h"
#include "ProjectT/System/Core/Managers/DataDam.h"
#include "ProjectT/System/Core/Managers/LoadingManager.h"
#include "ProjectT/System/Core/Managers/NotificationCenter.h"
#include "ProjectT/System/Core/Managers/QuestManager.h"
#include "ProjectT/System/Core/Managers/StageManager.h"
#include "ProjectT/System/Core/Objects/LevelObject/ExitPoint.h"
#include "ProjectT/System/Core/SaveGame/UserSaveGame.h"
#include "ProjectT/System/SinglePlay/Characters/Player/SPPyroMania.h"
#include "ProjectT/System/SinglePlay/Managers/SPEnemyVisionManager.h"
#include "ProjectT/System/SinglePlay/Managers/SPWidgetManager.h"


ASPGameModeBase::ASPGameModeBase() :
	SPEnemyVisionManager(nullptr),
	RemainingSeconds(0),
	ExitPoint(nullptr)
{
}

void ASPGameModeBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}


void ASPGameModeBase::InitializeEnemyCharacters()
{
	Super::InitializeEnemyCharacters();
}

void ASPGameModeBase::InitializePlayer()
{
	Super::InitializePlayer();
}

void ASPGameModeBase::InitializeUI()
{
	Super::InitializeUI();

	UWorld* World = UGlobalUtilsLibrary::GetValidWorld(this);
	ACorePlayerCharacter* CPC = GetPlayerCharacter();
	NMT_CHECKF(CoreWidgetManager);

	USPWidgetManager* SPWM = CastChecked<USPWidgetManager>(CoreWidgetManager);
	
	OnUpdateRemainingTime.AddDynamic(SPWM, &USPWidgetManager::ExecuteOnUpdateTime);
	
	CPC->PlayerDeadDone.AddDynamic(SPWM, &USPWidgetManager::ExecuteOnPlayerDead);
	CPC->OnRequestInteractIcon.BindDynamic(SPWM, &USPWidgetManager::ExecuteOnUpdateIndicator);
	CPC->RefreshInteractionVisibility.AddDynamic(SPWM, &USPWidgetManager::ExecuteOnInteractionIndicator);
	CPC->ItemSlotActivate.AddDynamic(SPWM, &USPWidgetManager::ExecuteOnUpdateSlotFocus);
	CPC->RefreshSkillCoolTime.AddDynamic(SPWM, &USPWidgetManager::ExecuteOnSkillUpdate);

	UPlayerInventoryComponent* PIC = CPC->GetPlayerInventory();
	NMT_CHECKF(PIC);

	PIC->UpdateSpendInventoryData.AddDynamic(SPWM, &USPWidgetManager::ExecuteOnUpdateConsumableSlot);
	PIC->UpdateGimmickInventoryData.AddDynamic(SPWM, &USPWidgetManager::ExecuteOnUpdateGimmickSlot);

	UPlayerStatusComponent* PSC = CPC->GetPlayerStatus();
	NMT_CHECKF(PSC);
	
	PSC->UpdatePlayerHpPercent.AddDynamic(SPWM, &USPWidgetManager::ExecuteOnUpdatePlayerHP);
	PSC->UpdatePlayerStaminaPercent.AddDynamic(SPWM, &USPWidgetManager::ExecuteOnUpdatePlayerStamina);
}

void ASPGameModeBase::InitializeExitPoint()
{
	AActor* Actor = nullptr;
	UWorld* World = UGlobalUtilsLibrary::GetValidWorld(this);
	Actor = UGameplayStatics::GetActorOfClass(World, AExitPoint::StaticClass());
	if(!Actor)
	{
		FRotator Rotator = FRotator();
		FVector SpawnPoint = FVector::ZeroVector; 
		TArray<AActor*> SpawnPoints;
		GetPlayerSpawnPoints(SpawnPoints);
		if(!SpawnPoints.IsEmpty())
		{
			UGlobalUtilsLibrary::ShuffleArray(PlayerSpawnPoints);
			SpawnPoint = SpawnPoints[0]->GetActorLocation();
		}
		else
		{
			FVector Center = FVector(0.f, 0.f, 0.f);
			FVector Extent = FVector(1000.f, 1000.f, 0.f);
			SpawnPoint = UKismetMathLibrary::RandomPointInBoundingBox(Center, Extent);
		}
		Actor = World->SpawnActor(AExitPoint::StaticClass(), &SpawnPoint, &Rotator);
	}

	ExitPoint = Cast<AExitPoint>(Actor);
	NMT_CHECKF(ExitPoint);
}

void ASPGameModeBase::SetStageTimeLimit()
{
	UStageManager* StageMgr = IGlobalObjectProvider::Execute_GetStageManager(GetGameInstance());
	NMT_CHECKF(StageMgr);
	FStageInfo CurrentStageInfo = StageMgr->GetCurrentStageInfo();
	RemainingSeconds = CurrentStageInfo.StageData.iTimeLimit;
}

void ASPGameModeBase::InitializeEnemyByFlag()
{
	for(AActor* Npc : NpcCharacters)
	{
		ACoreNpc* FindNpc = CastChecked<ACoreNpc>(Npc);	
		FindNpc->ActivateCharacter(FindNpc->GetActivatedOnStart());
	}
}

bool ASPGameModeBase::ChangedProcessEvent(const EProcessState ProcessState)
{
	bool bInitialized = Super::ChangedProcessEvent(ProcessState);
	if(!NMT_ENSURE(bInitialized)) return false;

	UWorld* World = UGlobalUtilsLibrary::GetValidWorld(this);

	UStageManager* SM = IGlobalObjectProvider::Execute_GetStageManager(GetGameInstance());
	NMT_CHECKF(SM);

	FStageInfo StageInfo = SM->GetCurrentStageInfo();
	ACoreGameState* CGS = UGlobalUtilsLibrary::GetGameStateChecked<ACoreGameState>(World);

	switch(ProcessState)
	{
		case EProcessState::Ready:
		{
			InitializePlayer();
			InitializeEnemyCharacters();
			InitializeEnemyByFlag();
			InitializeItemObject();
			InitializeExitPoint();
			InitializeUI();
			
			ULoadingManager* LoadingManager = GetGameInstance()->GetSubsystem<ULoadingManager>();
			NMT_CHECKF(LoadingManager);

			LoadingManager->ReceiveLoadingState(FOnLoadingStateStatic::CreateWeakLambda(this, [CGS](const ELoadingState LoadingState)
			{
				if(LoadingState == ELoadingState::End)
				{
					IProcessStateControl::Execute_ChangeProcessState(CGS, EProcessState::Intro);
				}
			}));
			LoadingManager->UnRegisterPreLoadableObject(this);
			
			break;
		}
		case EProcessState::Intro:
		{
			IProcessStateControl::Execute_ChangeProcessState(CGS, EProcessState::PreGame);
			break;
		}
		case EProcessState::PreGame:
		{
			ACorePlayerController* Controller = UGlobalUtilsLibrary::GetLocalPlayerControllerChecked<ACorePlayerController>(GetPlayerCharacter());
			Controller->InitializePlayerScreen();		

			QuestManager->InitializeQuestList(StageInfo.StageData, FOnQuestInitializedStatic::CreateLambda([this](const TArray<FObjective>& QuestList)
			{
				Execute_GetSPWidgetManager(this)->ExecuteOnQuestInitialized(QuestList);
			}));
			Execute_GetSPWidgetManager(this)->ExecuteStartGame();

			TSubclassOf<AActor> PlayerSkillClass;
			TSubclassOf<AActor> PlayerAtkClass;			
			GetPlayerCharacter()->GetSkillClass(PlayerAtkClass, PlayerSkillClass);

			UDataDam* DD = IGlobalObjectProvider::Execute_GetDataDam(GetGameInstance());
			NMT_CHECKF(DD);
			
			const UDataTable* FoundTable = DD->GetDataTableByEnum(EDataTableKey::Skill);
			FSkill* PlayerCurSkill = FoundTable->FindRow<FSkill>(FName(*FString::FromInt(GetPlayerCharacter()->GetPlayerStatus()->GetPreviousPlayerStatus().iCharSkillId)), TEXT(""));
			NMT_CHECKF(PlayerCurSkill);
			
			Execute_GetSPWidgetManager(this)->ExecuteOnSkillInitialized(*PlayerSkillClass, PlayerCurSkill->sSkillIconPath);
			InitializeMapData();
			IProcessStateControl::Execute_ChangeProcessState(CGS, EProcessState::GamePlaying);				
			break;
		}
		case EProcessState::GamePlaying:
		{
			QuestManager->UpdateQuestManager(this, FOnQuestChangedStatic::CreateLambda([this, SM](const EObjectiveTypes ObjectiveType, const FObjectiveSubTypeContext ObjectiveSubTypeContext, const bool IsQuestComplete)
			{
				Execute_GetSPWidgetManager(this)->ExecuteOnQuestUpdate(ObjectiveType, ObjectiveSubTypeContext, IsQuestComplete);
				if(ObjectiveType == EObjectiveTypes::Mission && IsQuestComplete) ExitPoint->ActivateExitPoint();
			}));

			ExitPoint->CompleteExitPoint(FOnCompleteExitPoint::CreateLambda([CGS]()
			{
				IProcessStateControl::Execute_ChangeProcessState(CGS, EProcessState::Victory);
			}));
			
			UNotificationCenter* NC = IGlobalObjectProvider::Execute_GetNotificationCenter(GetGameInstance());

			NC->AddObserverStatic(this, ENotificationTypes::Player_Dead, FNotificationCenterStatic::CreateLambda([this, CGS]()
			{
				IProcessStateControl::Execute_ChangeProcessState(CGS, EProcessState::Defeat);
			}));
			
			NC->AddObserverStatic(this, ENotificationTypes::GameState_Started, FNotificationCenterStatic::CreateLambda([this, World, CGS]()
			{
				ACorePlayerCharacter* CPC = GetPlayerCharacter();
				CPC->SetInputable(true);
				SetStageTimeLimit();
				World->GetTimerManager().SetTimer(StageRemainTimeHandle, [this, CGS]()
				{
					RemainingSeconds -= 1;
					OnUpdateRemainingTime.Broadcast(RemainingSeconds);

					if (RemainingSeconds == 0)
					{
						GetWorld()->GetTimerManager().ClearTimer(StageRemainTimeHandle);
						IProcessStateControl::Execute_ChangeProcessState(CGS, EProcessState::Defeat);
					}
				},1.0f, true);
			}));
			break;
		}
		case EProcessState::Victory:
		{
			if(QuestManager->IsAllClearedSubQuests())
			{
				UDataDam* DD = IGlobalObjectProvider::Execute_GetDataDam(GetGameInstance());
				NMT_CHECKF(DD);

				const UDataTable* FoundTable = DD->GetDataTableByEnum(EDataTableKey::CharacterUnlock);

				FCharacterUnlock* UnlockData = nullptr;
				for(TTuple<FName, uint8*> Data : FoundTable->GetRowMap())
				{
					UnlockData = reinterpret_cast<FCharacterUnlock*>(Data.Value);
					NMT_CHECKF(UnlockData);

					if(FROM_INT(UnlockData->iClearStage) == StageInfo.RowName) break;
					UnlockData = nullptr;
				}
				NMT_MSG_CHECKF(UnlockData, "Failed to retrieve data from the \"CharacterUnlock.csv\" iClearStage column using the index from another \"Stage.csv\" primary key[%s].", *StageInfo.RowName.ToString());
				UUserSaveGame::EmplaceUnlockCharacterData(FROM_INT(UnlockData->iPlayerCharId), true);
			}

			UNotificationCenter* NC = IGlobalObjectProvider::Execute_GetNotificationCenter(GetGameInstance());
			NC->Post(ENotificationTypes::GameState_Victory);
			NC->AddObserverStatic(this, ENotificationTypes::UI_VictoryActionCleared, FNotificationCenterStatic::CreateLambda([this, CGS]()
			{
				IProcessStateControl::Execute_ChangeProcessState(CGS, EProcessState::Reward);	
			}));
			Execute_GetSPWidgetManager(this)->ExecuteOnGameVictory();

			break;
		}
		case EProcessState::Defeat:
		{
			UNotificationCenter* NC = IGlobalObjectProvider::Execute_GetNotificationCenter(GetGameInstance());
			NC->Post(ENotificationTypes::GameState_Defeat);
			NC->AddObserverStatic(this, ENotificationTypes::UI_DefeatActionCleared, FNotificationCenterStatic::CreateLambda([this, World, SM]()
			{
				FString CurrentLevelName = UGameplayStatics::GetCurrentLevelName(World);
				
				UGameplayStatics::OpenLevel(World, *CurrentLevelName);
				
				//FIXME 데이터 테이블 레벨 패스정보가 달라서 크래시 발생
				// SM->PrepareLevelLoad(UGlobalUtilsLibrary::GetStringToEnum<EStage>(CurrentLevelName));	
			}));
			Execute_GetSPWidgetManager(this)->ExecuteOnGameDefeat();
			break;
		}
		case EProcessState::Reward:
		{
			IProcessStateControl::Execute_ChangeProcessState(CGS, EProcessState::Finish);
			break;	
		}
		case EProcessState::Finish:
		{
			SM->ForceLoadLevel(EStage::L_Lobby);
			break;	
		}
		default: case EProcessState::None: NMT_CHECKF(0);
	}
	return true;
}

USPWidgetManager* ASPGameModeBase::GetSPWidgetManager_Implementation()
{
	NMT_MSG_CHECKF(CoreWidgetManager, "Widget Manager is Null");
	
	USPWidgetManager* SPWM = Cast<USPWidgetManager>(CoreWidgetManager);
	NMT_MSG_CHECKF(SPWM, "SPWidgetManager Cast Failed");
	
	return SPWM;
}

void ASPGameModeBase::DebugGameTimePause(bool bPause)
{
	if(!StageRemainTimeHandle.IsValid()) return;

	UWorld* World = UGlobalUtilsLibrary::GetValidWorld(this);
	if(bPause) World->GetTimerManager().PauseTimer(StageRemainTimeHandle);
	else World->GetTimerManager().UnPauseTimer(StageRemainTimeHandle);
}

USPEnemyVisionManager* ASPGameModeBase::GetEnemyVisionManager_Implementation()
{
	return SPEnemyVisionManager;
}

UQuestManager* ASPGameModeBase::GetQuestManager_Implementation()
{
	return Super::GetQuestManager_Implementation();
}

bool ASPGameModeBase::HasFinishedLoading_Implementation()
{
	UWorld* World = UGlobalUtilsLibrary::GetValidWorld(this);
	ACoreGameState* CGS = UGlobalUtilsLibrary::GetGameStateChecked<ACoreGameState>(World);

	switch(EProcessState State = IProcessStateControl::Execute_GetProcessState(CGS))
	{
		case EProcessState::None: case EProcessState::Ready:
		{
			return false;
		}
		case EProcessState::Intro: case EProcessState::PreGame: case EProcessState::GamePlaying:
		case EProcessState::Victory: case EProcessState::Defeat: case EProcessState::Reward: case EProcessState::Finish:
		{
			return true;
		}
	}
	return false;
}

void ASPGameModeBase::InitializeMapData_Implementation()
{
}

void ASPGameModeBase::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	ULoadingManager* LoadingManager = GetGameInstance()->GetSubsystem<ULoadingManager>();
	NMT_CHECKF(LoadingManager);

	LoadingManager->RegisterPreLoadableObject(this);
}

void ASPGameModeBase::BeginPlay()
{
	Super::BeginPlay();
}

void ASPGameModeBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	GetWorld()->GetTimerManager().ClearTimer(StageRemainTimeHandle);

	CoreWidgetManager->ReleaseManager();
	CoreWidgetManager = nullptr;

	SPEnemyVisionManager->ReleaseManager();
	SPEnemyVisionManager = nullptr;
}

void ASPGameModeBase::InitializeManagers()
{
	Super::InitializeManagers();

	UWorld* World = GetWorld();
	NMT_CHECKF(World);
	
	if (!CoreWidgetManager)
	{
		CoreWidgetManager = NewObject<USPWidgetManager>(this);
		CoreWidgetManager->InitializeManager(this);
		NMT_CHECKF(CoreWidgetManager);
	}
	
	if(!SPEnemyVisionManager)
	{
		SPEnemyVisionManager = NewObject<USPEnemyVisionManager>(this);
		SPEnemyVisionManager->InitializeManager(World);
		NMT_CHECKF(SPEnemyVisionManager);
	}
}

void ASPGameModeBase::InitializeNotification()
{
	Super::InitializeNotification();
}