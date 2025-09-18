// Fill out your copyright notice in the Description page of Project Settings.


#include "CoreNpc.h"

#include "NiagaraComponent.h"
#include "AI/Controllers/AISightController.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "ProjectT/ProjectT.h"
#include "ProjectT/Data/Gen/GenerateEnumObjectiveSubTypes.h"
#include "ProjectT/System/Core/Characters/NPC/Animations/NpcAnimInstance.h"
#include "ProjectT/System/Core/Characters/Player/CorePlayerCharacter.h"
#include "ProjectT/System/Core/Common/GlobalUtilsLibrary.h"
#include "ProjectT/System/Core/Components/ObstacleFilterComponent.h"
#include "ProjectT/System/Core/Components/VisionConeComponent.h"
#include "ProjectT/System/Core/GameModes/WorldGameMode.h"
#include "ProjectT/System/Core/Interfaces/GlobalObjectProvider.h"
#include "ProjectT/System/Core/Managers/CoreCheatManager.h"
#include "ProjectT/System/Core/Managers/NotificationCenter.h"
#include "ProjectT/System/Core/Managers/QuestManager.h"
#include "ProjectT/System/SinglePlay/Components/SPNpcStatusComponent.h"

#define ACTIVATE_CHARACTER 1

ACoreNpc::ACoreNpc() :
	DefaultLocation(),
	DefaultRotation(),
	bReachedHeadRotation(false),
	bCanRotateHeadToTarget(false),
	bEnableRespawn(0),
	RespawnTime(0),
	BehaviorTree(nullptr),
	Blackboard(nullptr),
	SpringArmComponent(nullptr),
	VisionHead(nullptr),
	VisionConeComponent(nullptr),
	NpcStatusComponent(nullptr),
	AlertComponent(nullptr),
	CurrentState(ENpcState::Default),
	PerceptionMaxAge(3.f),
	NpcAnimInstance(nullptr),
	bActivatedOnStart(true),
	PrevState(ENpcState::Default),
	CurrentDoubtState(EDoubtState::None),
	EyeSocketName(TEXT("SKT_NPC_Vision")),
	AlertSphereProfileName(TEXT("SoundRange")),
	LoseSightOffset(10.f),
	HeadRotSpeedMultiplier(4.f),
	bCanTriggerAlert(true),
	bUnDetect(false),
	bFrustumCullingTimerSet(true),
	bActivate(true),
	bUseSocketTransformForVision(true)
{
	PrimaryActorTick.bCanEverTick = true;

	AIControllerClass = AAIControllerBase::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->GetNavMovementProperties()->bUseAccelerationForPaths = true;
	bUseControllerRotationYaw = false;

	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	GetMesh()->SetAnimInstanceClass(UNpcAnimInstance::StaticClass());

	SubRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SubRootComponent"));
	WidgetRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("WidgetRootComponent"));
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	VisionHead = CreateDefaultSubobject<USceneComponent>(TEXT("VisionHead"));
	VisionConeComponent = CreateDefaultSubobject<UVisionConeComponent>(TEXT("VisionConeComponent"));
	AlertComponent = CreateDefaultSubobject<UObstacleFilterComponent>(TEXT("AlertComponent"));
	NpcStatusComponent = CreateDefaultSubobject<USPNpcStatusComponent>(TEXT("NpcStatusComponent"));
	NoiseFXComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NoiseFXComponent"));
	NMT_CHECKF(WidgetRootComponent && SubRootComponent && SpringArmComponent && VisionHead && VisionConeComponent && AlertComponent && NpcStatusComponent && NoiseFXComponent)
	
	SubRootComponent->SetupAttachment(RootComponent);
	WidgetRootComponent->SetupAttachment(RootComponent);

	SpringArmComponent->SetupAttachment(SubRootComponent);
	SpringArmComponent->bDoCollisionTest = false;
	SpringArmComponent->TargetArmLength = 0.f;
	SpringArmComponent->bInheritRoll = false;
	SpringArmComponent->bInheritPitch = false;
	SpringArmComponent->bEnableCameraLag = true;
	SpringArmComponent->CameraLagSpeed = 0.f;
	
	SpringArmComponent->bEnableCameraRotationLag = true;
	SpringArmComponent->CameraRotationLagSpeed = 7.f;
	SpringArmComponent->SetCanEverAffectNavigation(false);

	VisionHead->SetupAttachment(SpringArmComponent);
	VisionConeComponent->SetupAttachment(SpringArmComponent);

	AlertComponent->SetupAttachment(SubRootComponent);
	AlertComponent->DeactivateFiltering();
	AlertComponent->SetCollisionProfileName(AlertSphereProfileName);

	NoiseFXComponent->SetupAttachment(SubRootComponent);
	NoiseFXComponent->bAutoActivate = false;
	
	if(HeadRotationPatterns.IsEmpty())
		SetupHeadRotationPattern();
}

void ACoreNpc::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	DefaultLocation = GetActorLocation();
	DefaultRotation = GetActorRotation();
}

void ACoreNpc::BeginPlay()
{
	Super::BeginPlay();
	
	ActivateCharacter(false);
	InitializeProperty();
	InitializeEvents();
}

void ACoreNpc::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void ACoreNpc::Enable_Implementation()
{
	IPoolableActor::Enable_Implementation();
}

void ACoreNpc::Disable_Implementation()
{
	IPoolableActor::Disable_Implementation();
}

void ACoreNpc::Pushed_Implementation()
{
	IPoolableActor::Pushed_Implementation();
}

void ACoreNpc::Obtained_Implementation()
{
	IPoolableActor::Obtained_Implementation();
}

void ACoreNpc::GetCurrentEffectTypes_Implementation(TArray<EEffectSubTypes>& OutEffectSubTypes)
{
	TArray<EEffectSubTypes> LocReturnCurrentEffects;
	EffectTimeMap.GetKeys(LocReturnCurrentEffects);
	if(LocReturnCurrentEffects.Num() > 0)
	{
		OutEffectSubTypes = LocReturnCurrentEffects;
	}
	else
	{
		LocReturnCurrentEffects.Add(EEffectSubTypes::None);
		OutEffectSubTypes = LocReturnCurrentEffects;
	}
}

void ACoreNpc::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if(!bFrustumCullingTimerSet)
	{
		bFrustumCullingTimerSet = true;
		UWorld* World = UGlobalUtilsLibrary::GetValidWorld(this);
		World->GetTimerManager().SetTimer(UpdateActiveComponentHandle, this, &ACoreNpc::UpdateActiveComponentInFrustum, DeltaTime, true);
	}

	if(HeadRotationInfo.bOverrideHeadRotation)
		UpdateHeadRotation(DeltaTime);

	if(HeadRotationInfo.bChangeAlpha)
		UpdateHeadRotationAlpha(DeltaTime);

	SetSpringArmRotation();
	if(VisionConeComponent->IsDrawing())
		VisionConeComponent->UpdateVision(VisionHead->GetComponentRotation());
}

void ACoreNpc::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	UWorld* World = UGlobalUtilsLibrary::GetValidWorld(this);
	World->GetTimerManager().ClearTimer(UpdateActiveComponentHandle);
	World->GetTimerManager().ClearTimer(RespawnTimerHandle);
	World->GetTimerManager().ClearTimer(CompleteAlertTimerHandle);
	World->GetTimerManager().ClearTimer(EffectTimerHandle);

	OnNpcDead.Clear();
	OnNpcRespawn.Clear();
	OnNpcIndicatorChanged.Clear();
}

void ACoreNpc::GetActorEyesViewPoint(FVector& Location, FRotator& Rotation) const
{
	if(!VisionHead) return;
	Location = VisionHead->GetComponentLocation();
	Rotation = VisionHead->GetComponentRotation();
}

FEnemyNpc ACoreNpc::GetCurrentNpcStatus() const
{
	if(!NMT_ENSURE(NpcStatusComponent))
		return FEnemyNpc();

	return NpcStatusComponent->GetCurrentNpcStatus();
}

void ACoreNpc::InitializeStatus(const FEnemyNpc InNpcStatus)
{
	if(!NMT_ENSURE(NpcStatusComponent))
		return;

	bool bNpcDataValid = !InNpcStatus.IsDefault();
	if(!NMT_ENSURE(bNpcDataValid)) return;

#if WITH_EDITOR
	NpcStatusComponent->InitializeStatus(InNpcStatus);
#endif
}

void ACoreNpc::InitializeProperty()
{
	NpcAnimInstance = Cast<UNpcAnimInstance>(GetMesh()->GetAnimInstance());
	NMT_CHECKF(NpcAnimInstance && NpcStatusComponent)
	
	FObstacleFilterInfo FilterInfo;
	FilterInfo.SphereRadius = NpcStatusComponent->GetStatusNpcNoise();
	FilterInfo.CheckOutOfRange = false;
	AlertComponent->InitializeProperty(FilterInfo);
	AlertComponent->DeactivateFiltering();
}

void ACoreNpc::InitializeEvents()
{
	UNotificationCenter* NC = IGlobalObjectProvider::Execute_GetNotificationCenter(UGameplayStatics::GetGameInstance(this));

	NC->AddObserverStatic(this, ENotificationTypes::GameState_Victory, FNotificationCenterStatic::CreateWeakLambda(this, [this]()
	{
		ResetStateAfterGameEnd();
	}));
	NC->AddObserverStatic(this, ENotificationTypes::GameState_Defeat, FNotificationCenterStatic::CreateWeakLambda(this, [this]()
	{
		ResetStateAfterGameEnd();;
	}));
	// FIXME 
	// NC->AddObserverStatic(this, ENotificationTypes::UI_DefeatActionCleared, FNotificationCenterStatic::CreateWeakLambda(this, [this]()
	// {
	// 	SetActorLocationAndRotation(DefaultLocation, DefaultRotation);
	// 	SetActorHiddenInGame(true);
	// 	NMT_LOG("UI_DefeatActionCleared")
	// }));
	// NC->AddObserverStatic(this, ENotificationTypes::UI_VictoryActionCleared, FNotificationCenterStatic::CreateWeakLambda(this, [this]()
	// {
	// 	SetActorLocationAndRotation(DefaultLocation, DefaultRotation);
	// 	SetActorHiddenInGame(true);
	// 	NMT_LOG("UI_VictoryActionCleared")
	// }));
}

void ACoreNpc::Respawn()
{
	Super::Respawn();
	
	SetActorEnableCollision(true);
	GetMesh()->bPauseAnims = false;
	GetAIControllerBase()->StartAI();
	SetActorHiddenInGame(false);
	SetActorTickEnabled(true);
	NpcStatusComponent->ReInitializeStatus();

	ChangeNpcState(ENpcState::Default);
	OnNpcRespawn.Broadcast();
}

void ACoreNpc::ChangeHeadRotationState(const EHeadRotationType InHeadRotationType)
{
	bReachedHeadRotation = false;
	switch(InHeadRotationType)
	{
		case EHeadRotationType::Pause:
		{
			NpcStatusComponent->SetStatusNpcRotationSpeed(NpcStatusComponent->GetPreviousNpcStatus().fRotationSpeed);
			HeadRotationInfo.bOverrideHeadRotation = false;
			break;
		}
		case EHeadRotationType::Search: case EHeadRotationType::LoopSearch:
		{
			NpcStatusComponent->SetStatusNpcRotationSpeed(NpcStatusComponent->GetPreviousNpcStatus().fRotationSpeed);
			CurrentPhaseLists = *HeadRotationPatterns.Find(InHeadRotationType);
			if(!CurrentPhaseLists.IsEmpty())
			{
				HeadRotationInfo.CurrentPhaseIdx = 0;
				HeadRotationInfo.bOverrideHeadRotation = true;
			}
			break;
		}
		case EHeadRotationType::Guard:
		{
			NpcStatusComponent->SetStatusNpcRotationSpeed(NpcStatusComponent->GetPreviousNpcStatus().fRotationSpeed * HeadRotSpeedMultiplier);
			CurrentPhaseLists = *HeadRotationPatterns.Find(InHeadRotationType);
			if(!CurrentPhaseLists.IsEmpty())
			{
				HeadRotationInfo.CurrentPhaseIdx = 0;
				HeadRotationInfo.bOverrideHeadRotation = true;
			}
			break;
		}
		case EHeadRotationType::TurnToTarget:
		{
			NpcStatusComponent->SetStatusNpcRotationSpeed(NpcStatusComponent->GetPreviousNpcStatus().fRotationSpeed * HeadRotSpeedMultiplier);
			HeadRotationInfo.bOverrideHeadRotation = true;
			break;
		}
		case EHeadRotationType::None:
		{
			NpcStatusComponent->SetStatusNpcRotationSpeed(NpcStatusComponent->GetPreviousNpcStatus().fRotationSpeed);
			break;
		}
	}
	
	HeadRotationInfo.HeadRotationType = InHeadRotationType;
}

void ACoreNpc::SetHeadRotationEnable(const bool InbEnable)
{
	HeadRotationInfo.bChangeAlpha = true;

	InbEnable ? HeadRotationInfo.TargetAlpha = 1.f : HeadRotationInfo.TargetAlpha = 0.f;
}

bool ACoreNpc::IsAggressiveState(const ENpcState InState) const
{
	return InState == ENpcState::Chase || InState == ENpcState::Attack;
}

bool ACoreNpc::IsDamagedState(const ENpcState InState) const
{
	return (InState == ENpcState::Hit || InState == ENpcState::Dead) || GetNpcStatusComponent()->IsDead();
}

void ACoreNpc::SetupHeadRotationPattern()
{
	TArray<EHeadRotationPhase> List;
	
	List.Append({EHeadRotationPhase::TurnRight, EHeadRotationPhase::TurnLeft, EHeadRotationPhase::TurnForward});
	HeadRotationPatterns.Add(EHeadRotationType::Search, List);

	List.Empty();
	List.Append({EHeadRotationPhase::TurnRight, EHeadRotationPhase::TurnLeft, EHeadRotationPhase::TurnForward});
	HeadRotationPatterns.Add(EHeadRotationType::Guard, List);

	List.Empty();
	List.Append({EHeadRotationPhase::TurnRight, EHeadRotationPhase::TurnLeft});
	HeadRotationPatterns.Add(EHeadRotationType::LoopSearch, List);

}

void ACoreNpc::UpdateHeadRotation(float InDeltaTime)
{
	if(!NMT_ENSURE(NpcStatusComponent))
		return;

	switch(HeadRotationInfo.HeadRotationType)
	{
		case EHeadRotationType::LoopSearch:
		{
			if(!NMT_MSG_ENSURE(CurrentPhaseLists.IsValidIndex(HeadRotationInfo.CurrentPhaseIdx), "Not ValidIndex in CurrentPhaseLists %d", HeadRotationInfo.CurrentPhaseIdx))
			{
				ResetHeadRotation();
				bReachedHeadRotation = true;
				HeadRotationInfo.bOverrideHeadRotation = false;
				return;
			}
			
			if(CurrentPhaseLists[HeadRotationInfo.CurrentPhaseIdx] == EHeadRotationPhase::TurnRight)
			{
				HeadRotationInfo.TargetRotation = FRotator(NpcAnimInstance->ForwardHeadRotation.Pitch, NpcAnimInstance->ForwardHeadRotation.Yaw + NpcStatusComponent->GetStatusNpcHalfRotationAngle(), NpcAnimInstance->ForwardHeadRotation.Roll);
			}
			else if(CurrentPhaseLists[HeadRotationInfo.CurrentPhaseIdx] == EHeadRotationPhase::TurnLeft)
			{
				HeadRotationInfo.TargetRotation = FRotator(NpcAnimInstance->ForwardHeadRotation.Pitch, NpcAnimInstance->ForwardHeadRotation.Yaw - NpcStatusComponent->GetStatusNpcHalfRotationAngle(), NpcAnimInstance->ForwardHeadRotation.Roll);
			}

			if(NpcAnimInstance->HeadRotation.Equals(HeadRotationInfo.TargetRotation, 0.1f))
			{
				HeadRotationInfo.CurrentPhaseIdx == 0 ? HeadRotationInfo.CurrentPhaseIdx++ : HeadRotationInfo.CurrentPhaseIdx--;
			}
			NpcAnimInstance->HeadRotation = FMath::RInterpTo(NpcAnimInstance->HeadRotation, HeadRotationInfo.TargetRotation, InDeltaTime, NpcStatusComponent->GetStatusNpcRotationSpeed());
			break;
		}
		case EHeadRotationType::Search: case EHeadRotationType::Guard:
		{
			if(!NMT_MSG_ENSURE(CurrentPhaseLists.IsValidIndex(HeadRotationInfo.CurrentPhaseIdx), "Not ValidIndex in CurrentPhaseLists %d", HeadRotationInfo.CurrentPhaseIdx))
			{
				ResetHeadRotation();
				bReachedHeadRotation = true;
				HeadRotationInfo.bOverrideHeadRotation = false;
				return;
			}
			
			if(CurrentPhaseLists[HeadRotationInfo.CurrentPhaseIdx] == EHeadRotationPhase::TurnRight)
			{
				HeadRotationInfo.TargetRotation = FRotator(NpcAnimInstance->ForwardHeadRotation.Pitch, NpcAnimInstance->ForwardHeadRotation.Yaw + NpcStatusComponent->GetStatusNpcHalfRotationAngle(), NpcAnimInstance->ForwardHeadRotation.Roll);
			}
			else if(CurrentPhaseLists[HeadRotationInfo.CurrentPhaseIdx] == EHeadRotationPhase::TurnLeft)
			{
				HeadRotationInfo.TargetRotation = FRotator(NpcAnimInstance->ForwardHeadRotation.Pitch, NpcAnimInstance->ForwardHeadRotation.Yaw - NpcStatusComponent->GetStatusNpcHalfRotationAngle(), NpcAnimInstance->ForwardHeadRotation.Roll);
			}
			else if(CurrentPhaseLists[HeadRotationInfo.CurrentPhaseIdx] == EHeadRotationPhase::TurnForward)
			{
				HeadRotationInfo.TargetRotation = NpcAnimInstance->ForwardHeadRotation;
			}

			if(NpcAnimInstance->HeadRotation.Equals(HeadRotationInfo.TargetRotation, 0.1f))
			{
				HeadRotationInfo.CurrentPhaseIdx++;
				if(HeadRotationInfo.CurrentPhaseIdx >= CurrentPhaseLists.Num())
				{
					bReachedHeadRotation = true;
					ResetHeadRotation();
				}
			}
			NpcAnimInstance->HeadRotation = FMath::RInterpTo(NpcAnimInstance->HeadRotation, HeadRotationInfo.TargetRotation, InDeltaTime, NpcStatusComponent->GetStatusNpcRotationSpeed());
			break;
		}
		case EHeadRotationType::TurnToTarget:
		{
			if(NpcAnimInstance->HeadRotation.Equals(HeadRotationInfo.TargetRotation, 0.1f))
			{
				bReachedHeadRotation = true;
				HeadRotationInfo.bOverrideHeadRotation = false;
				ResetHeadRotation();
			}
			NpcAnimInstance->HeadRotation = FMath::RInterpTo(NpcAnimInstance->HeadRotation, HeadRotationInfo.TargetRotation, InDeltaTime, NpcStatusComponent->GetStatusNpcRotationSpeed());
			break;
		}
		case EHeadRotationType::Pause: case EHeadRotationType::None:
			break;
		default : NMT_MSG_ENSURE(0, "Not Valid HeadRotation Type");
	}
}

void ACoreNpc::UpdateHeadRotationAlpha(float InDeltaTime)
{
	if(FMath::IsNearlyEqual(NpcAnimInstance->HeadRotAlpha, HeadRotationInfo.TargetAlpha))
	{
		NpcAnimInstance->HeadRotAlpha = HeadRotationInfo.TargetAlpha;
		HeadRotationInfo.bChangeAlpha = false;
	}

	NpcAnimInstance->HeadRotAlpha = FMath::FInterpTo(NpcAnimInstance->HeadRotAlpha, HeadRotationInfo.TargetAlpha, InDeltaTime, 5.f);
}

void ACoreNpc::ResetHeadRotation()
{
	HeadRotationInfo.CurrentPhaseIdx = 0;
	HeadRotationInfo.HeadRotationType = EHeadRotationType::None;
}

void ACoreNpc::SetUnDetectTarget(const bool InbUnDetect)
{
	bUnDetect = InbUnDetect;
}

void ACoreNpc::PostAlert() const
{
	AlertComponent->DeactivateFiltering();
}

bool ACoreNpc::CheckEffectTime(const FEffect& InEffect)
{
	bool LocbRefreshEffect = false;
	EEffectSubTypes LocEffectSubType = UGlobalUtilsLibrary::GetStringToEnum<EEffectSubTypes>(InEffect.sEffect);
	if(EffectTimeMap.Find(LocEffectSubType))
	{
		if(EffectTimeMap.Find(LocEffectSubType)->EffectValue > InEffect.fEffectValue)
			return LocbRefreshEffect;
		if(EffectTimeMap.Find(LocEffectSubType)->EffectValue == InEffect.fEffectValue)
		{
			if(EffectTimeMap.Find(LocEffectSubType)->RemainTime < InEffect.fEffectDuration)
				LocbRefreshEffect = true;
		}
		else
			LocbRefreshEffect = true;
	}
	else
		LocbRefreshEffect = true;
	
	return LocbRefreshEffect;
}

void ACoreNpc::AddEffectTimeMap(const FEffect& InEffect)
{
	FNpcEffectValue LocEffectValue;
	EEffectSubTypes LocEffectSubType = UGlobalUtilsLibrary::GetStringToEnum<EEffectSubTypes>(InEffect.sEffect);
	if(EffectTimeMap.Find(LocEffectSubType))
	{
		if(EffectTimeMap.Find(LocEffectSubType)->EffectValue > InEffect.fEffectValue) return;
		if(EffectTimeMap.Find(LocEffectSubType)->EffectValue == InEffect.fEffectValue)
		{
			if(EffectTimeMap.Find(LocEffectSubType)->RemainTime < InEffect.fEffectDuration)
			{
				LocEffectValue.RemainTime = InEffect.fEffectDuration;
				LocEffectValue.MaxTime = InEffect.fEffectDuration;
				LocEffectValue.EffectValue = InEffect.fEffectValue;
				EffectTimeMap.Add(LocEffectSubType, LocEffectValue);
			}	
		}
		else
		{
			LocEffectValue.RemainTime = InEffect.fEffectDuration;
			LocEffectValue.MaxTime = InEffect.fEffectDuration;
			LocEffectValue.EffectValue = InEffect.fEffectValue;
			EffectTimeMap.Add(LocEffectSubType, LocEffectValue);
		}
	}
	else
	{
		LocEffectValue.RemainTime = InEffect.fEffectDuration;
		LocEffectValue.MaxTime = InEffect.fEffectDuration;
		LocEffectValue.EffectValue = InEffect.fEffectValue;
		EffectTimeMap.Add(LocEffectSubType, LocEffectValue);
	}
	if(!GetWorld()->GetTimerManager().IsTimerActive(EffectTimerHandle))
		GetWorld()->GetTimerManager().SetTimer(EffectTimerHandle, this, &ACoreNpc::EffectTimeDown, GetWorld()->DeltaTimeSeconds, true);
}


void ACoreNpc::EffectTimeDown()
{
	TArray<EEffectSubTypes> LocEffectSubType;
	EffectTimeMap.GetKeys(LocEffectSubType);
	FEffect LocRemoveEffect;
	for(EEffectSubTypes LocKey : LocEffectSubType)
	{
		float LocCurRemainTime = EffectTimeMap.Find(LocKey)->RemainTime;
		LocCurRemainTime = LocCurRemainTime - GetWorld()->DeltaTimeSeconds;
		if(LocCurRemainTime <= 0.f)
		{
			TArray<FEffect> LocRemoveEffects;
			LocRemoveEffect.sEffect = EnumToString(LocKey);
			LocRemoveEffect.fEffectValue = EffectTimeMap.Find(LocKey)->EffectValue;
			LocRemoveEffects.Add(LocRemoveEffect);
			ApplyRecoveryEvent_Implementation(LocRemoveEffects);
		}
		else
		{
			EffectTimeMap.Find(LocKey)->RemainTime = LocCurRemainTime; 
		}		
	}	
}

void ACoreNpc::SetReturnLocationFromDefault() const
{
	if(GetNpcState() == ENpcState::Default)
		GetAIControllerBase()->UpdateReturnLocation(GetActorLocation());
}

void ACoreNpc::ChangeIndicatorState(const ENpcState InState)
{
	if(NpcStatusComponent->IsDead())
		return;
	
	ENpcIndicatorState NewIndicatorState = LastIndicatorState;
	
	switch(InState)
	{
		case ENpcState::Doubt: case ENpcState::Guard:
		{
			NewIndicatorState = ENpcIndicatorState::Question;
			break;
		}
		case ENpcState::Chase: case ENpcState::Attack:
		{
			NewIndicatorState = ENpcIndicatorState::Exclamation;
			break;
		}
		case ENpcState::Default: case ENpcState::Returning: case ENpcState::Hit: case ENpcState::Dead:
			break;
		default: NMT_LOG("Not Valid Type")
	}

	if(NewIndicatorState != LastIndicatorState)
	{
		LastIndicatorState = NewIndicatorState;
		OnNpcIndicatorChanged.Broadcast(NewIndicatorState);
	}
}

void ACoreNpc::ChangeToChaseInDoubtGauge() 
{
	GetAIControllerBase()->UpdateReturnLocation(GetActorLocation());
	ChangeNpcState(ENpcState::Chase);
	ApplyStateToQuest(ENpcState::Chase);
}

void ACoreNpc::DebugToggleFrustum()
{
	UCoreCheatManager* CCM = UGlobalUtilsLibrary::GetSharedCoreCheatManager(this);
	UWorld* World = UGlobalUtilsLibrary::GetValidWorld(this);
	if(UpdateActiveComponentHandle.IsValid())
	{
		if(CCM->GetDebugStatus().bDebugVision)
		{
			World->GetTimerManager().PauseTimer(UpdateActiveComponentHandle);
			VisionConeComponent->ToggleDraw(true);
		}
		else
		{
			VisionConeComponent->ToggleDraw(false);
			World->GetTimerManager().UnPauseTimer(UpdateActiveComponentHandle);
		}
	}
}

class AAIControllerBase* ACoreNpc::GetAIControllerBase() const
{
	AAIControllerBase* AIC = Cast<AAIControllerBase>(GetController());
	NMT_CHECKF(AIC)

	return AIC;
}

bool ACoreNpc::CanDetectTargetFromNoise() const
{
	return !bUnDetect && !GetNpcStatusComponent()->IsDead() && !GetAIControllerBase()->GetTargetActor() && CurrentState != ENpcState::Attack;
}

void ACoreNpc::ApplyStateToQuest(const ENpcState InState)
{
	UWorld* World = UGlobalUtilsLibrary::GetValidWorld(this);
	AWorldGameMode* GM = UGlobalUtilsLibrary::GetGameModeChecked<AWorldGameMode>(World);
	UQuestManager* QM = IWorldObjectProvider::Execute_GetQuestManager(GM);
	NMT_CHECKF(QM);
	
	switch(InState)
	{
		case ENpcState::Chase:
		{
			NMT_LOG("Avoid")
			QM->CheckObjectiveSatisfied(EObjectiveSubTypes::Avoid);	
			break;
		}
		case ENpcState::Dead:
		{
			QM->CheckObjectiveSatisfied(EObjectiveSubTypes::Kill);
			break;
		}
		case ENpcState::Guard:
		case ENpcState::Hit:
		case ENpcState::Attack:
		case ENpcState::Default:
		case ENpcState::Returning:
		case ENpcState::Doubt:
		default: NOT_IMPLEMENTED(); break;
	}
}

void ACoreNpc::Alert(const FVector& InTargetLocation)
{
	if(!bCanTriggerAlert || GetNpcStatusComponent()->IsDead())
		return;

	NMT_CHECKF(NpcStatusComponent)
	NMT_LOG("Alert")
	
	bCanTriggerAlert = false;
	AlertComponent->ActivateFiltering();
	
	TArray<AActor*> DetectedNpcs = AlertComponent->GetNonBlockedActors(InTargetLocation);
	for(AActor* Npc : DetectedNpcs)
	{
		ACoreNpc* OtherNpc = Cast<ACoreNpc>(Npc);
		if(!OtherNpc)
			continue;
		
		OtherNpc->SetDetectLocationFromNoise(InTargetLocation);
	}
	
	NoiseFXComponent->SetFloatParameter(FName("Wave Radius"), NpcStatusComponent->GetStatusNpcNoise());
	NoiseFXComponent->Activate(true);
	UWorld* World = UGlobalUtilsLibrary::GetValidWorld(this);
	World->GetTimerManager().SetTimer(CompleteAlertTimerHandle, this, &ACoreNpc::PostAlert, 0.5f, false);
}

void ACoreNpc::SetDetectTargetFromNoise(AActor* InTarget)
{
	if(!CanDetectTargetFromNoise())
		return;
	
	GetAIControllerBase()->UpdateTargetInfo(InTarget);
	HandleForceDetectFromNoise(true);
	
	NMT_LOG("SetDetectTargetFromNoise")
}

void ACoreNpc::SetDetectLocationFromNoise(const FVector& InTargetLocation)
{
	if(!CanDetectTargetFromNoise())
		return;
	
	GetAIControllerBase()->UpdateTargetLocation(InTargetLocation);
	GetAIControllerBase()->UpdateTargetDirection((InTargetLocation - GetActorLocation()).GetSafeNormal());
	HandleForceDetectFromNoise(false);
	
	NMT_LOG("SetDetectLocationFromNoise")
}

void ACoreNpc::UpdateDoubtVisualize(const float InCurrentDoubtTime)
{
	double GaugeLength = UKismetMathLibrary::MapRangeClamped(InCurrentDoubtTime, 0.f, NpcStatusComponent->GetStatusPreviousNpcDoubtTime(), 20.f, NpcStatusComponent->GetStatusNpcSightLength());
	VisionConeComponent->UpdateVisionGauge(GaugeLength);

	AActor* Target = GetAIControllerBase()->GetTargetActor();
	if(!Target)
		return;
	
	if(GaugeLength >= GetHorizontalDistanceTo(Target))
	{
		ChangeToChaseInDoubtGauge();
	}
}

void ACoreNpc::UpdateDoubtTime(const float InCurrentDoubtTime) 
{
	NMT_CHECKF(NpcStatusComponent)
	NpcStatusComponent->UpdateCurrentDoubtTime(FMath::Clamp(InCurrentDoubtTime, 0.f, NpcStatusComponent->GetPreviousNpcStatus().fDoubtTime));
	UpdateDoubtVisualize(InCurrentDoubtTime);
}

void ACoreNpc::ChangeDoubtState(const EDoubtState InDoubtState)
{
	CurrentDoubtState = InDoubtState;
	GetAIControllerBase()->UpdateDoubtState(InDoubtState);

	switch(InDoubtState)
	{
		case EDoubtState::None:
		{
			UpdateDoubtTime(0.f);
			UpdateDoubtVisualize(0.f);

			break;
		}
		case EDoubtState::Increasing: case EDoubtState::Decreasing: break;
		default: NOT_IMPLEMENTED();
	}
}

void ACoreNpc::SetHeadTargetRotation(const FVector& InTargetLocation)
{
	NMT_CHECKF(NpcStatusComponent)
	FVector EyeLocation = GetMesh()->GetSocketLocation(EyeSocketName);
	FRotator EyeRotation = GetMesh()->GetSocketRotation(EyeSocketName);
	FRotator WorldLookRotator = UKismetMathLibrary::FindLookAtRotation(EyeLocation, InTargetLocation);
	FRotator CompLookRotator = WorldLookRotator - EyeRotation;
	
	float TargetYaw = FMath::Clamp(NpcAnimInstance->ForwardHeadRotation.Yaw + CompLookRotator.Yaw, NpcAnimInstance->ForwardHeadRotation.Yaw - NpcStatusComponent->GetStatusNpcHalfRotationAngle(), NpcAnimInstance->ForwardHeadRotation.Yaw + NpcStatusComponent->GetStatusNpcHalfRotationAngle());
	HeadRotationInfo.TargetRotation = FRotator(NpcAnimInstance->ForwardHeadRotation.Pitch, TargetYaw, NpcAnimInstance->ForwardHeadRotation.Roll);
}

TArray<EEffectSubTypes> ACoreNpc::GetCurrentEffects() const
{
	TArray<EEffectSubTypes> LocReturnCurrentEffects;
	EffectTimeMap.GetKeys(LocReturnCurrentEffects);
	if(LocReturnCurrentEffects.Num() > 0)
		return LocReturnCurrentEffects;
	
	LocReturnCurrentEffects.Add(EEffectSubTypes::None);
	return LocReturnCurrentEffects;
}

void ACoreNpc::HandleForceDetectFromNoise(const bool InbDetectTarget)
{
	bCanTriggerAlert = false;
	SetReturnLocationFromDefault();
	if(!IsDamagedState(CurrentState))
	{
		ChangeNpcState(ENpcState::Chase);
		if(InbDetectTarget && (PrevState != ENpcState::Attack && PrevState != ENpcState::Hit))
		{
			ApplyStateToQuest(ENpcState::Chase);
		}
	}
}

void ACoreNpc::ChangeNpcState(const ENpcState InState)
{
	PrevState = CurrentState;
	if(CurrentState != InState)
	{
		switch(InState)
		{
			case ENpcState::Guard:
			{
				bCanTriggerAlert = true;
				break;
			}
			case ENpcState::Chase: case ENpcState::Attack: case ENpcState::Default: case ENpcState::Doubt: case ENpcState::Returning: case ENpcState::Hit: case ENpcState::Dead:
				break;
			default: NMT_LOG("Not Valid ENpcState");
		}
		
		CurrentState = InState;
		GetAIControllerBase()->ChangeNpcStateKey(InState);
	}
}

void ACoreNpc::StartDisableNpc()
{
	OnNpcDead.Broadcast();
	SetActorEnableCollision(false);
	GetCharacterMovement()->SetMovementMode(MOVE_None);
	GetAIControllerBase()->StopAI("Death");

	NMT_LOG("DisableNpc")
}

void ACoreNpc::CompleteDisableNpc()
{
	UWorld* World = UGlobalUtilsLibrary::GetValidWorld(this);
	SetActorHiddenInGame(true);
	SetActorTickEnabled(false);
	SetActorLocation(DefaultLocation);
	SetActorRotation(DefaultRotation);
	ChangeDoubtState(EDoubtState::None);
	LastIndicatorState = ENpcIndicatorState::None;

	if(bEnableRespawn)
	{
		World->GetTimerManager().SetTimer(RespawnTimerHandle, this, &ACoreNpc::Respawn, RespawnTime, false);
	}
}

void ACoreNpc::ResetStateAfterGameEnd()
{
	if(!bActivate)
		return; 

	AAIControllerBase* AIC = GetAIControllerBase();
	if(AIC->GetTargetActor())
		AIC->UpdateTargetInfo(nullptr);
	
	ChangeNpcState(ENpcState::Default);
	ChangeDoubtState(EDoubtState::None);
	NpcStatusComponent->ReInitializeStatus();
	LastIndicatorState = ENpcIndicatorState::None;
	AIC->StopAI("Ending", false);
	SetActorTickEnabled(false);
}

void ACoreNpc::ActivateCharacter(const bool InbEnable)
{
	SetActorTickEnabled(InbEnable);
	SetActorHiddenInGame(!InbEnable);
	
	TArray<USceneComponent*> Components;
	GetComponents(Components);

	for (USceneComponent* Component : Components)
	{
		Component->SetComponentTickEnabled(InbEnable);
	}

	if(InbEnable)
	{
		bFrustumCullingTimerSet = false;
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::Type::QueryAndPhysics);
		GetAIControllerBase()->ActivateBehaviorTree(this);
	}
	else
	{
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	bActivate = InbEnable;
}

void ACoreNpc::SetSpringArmRotation()
{
	if(!bUseSocketTransformForVision)
		return;
	
	FRotator ArmRotator = SpringArmComponent->GetComponentRotation();
	SpringArmComponent->SetWorldRotation(UKismetMathLibrary::MakeRotator(ArmRotator.Roll, ArmRotator.Pitch, GetMesh()->GetSocketRotation(EyeSocketName).Yaw));
}

void ACoreNpc::SetVisionMode(EVisionMode InSightMode)
{
	NOT_IMPLEMENTED()
}

void ACoreNpc::UpdateActiveComponentInFrustum() const
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	NMT_CHECKF(PC)
	
	if(ULocalPlayer* LocalPlayer = PC->GetLocalPlayer())
	{
		FSceneViewProjectionData ProjectionData;
		if(LocalPlayer->GetProjectionData(LocalPlayer->ViewportClient->Viewport, ProjectionData))
		{
			const FMatrix ViewProjectionMatrix = ProjectionData.ComputeViewProjectionMatrix();

			FConvexVolume Frustum;
			GetViewFrustumBounds(Frustum, ViewProjectionMatrix, true);
			bool bInFrustum = Frustum.IntersectSphere(RootComponent->GetComponentLocation(), VisionConeComponent->SightRange);
			bool bDoNotDraw = IsDamagedState(CurrentState) || bUnDetect;
			bDoNotDraw |= IsAggressiveState(CurrentState);
		
			if(VisionConeComponent->IsAllowedDrawing())
				VisionConeComponent->ToggleDraw(bInFrustum && !bDoNotDraw);				
		}
	}
}

class USPNpcStatusComponent* ACoreNpc::GetNpcStatusComponent() const
{
	NMT_CHECKF(NpcStatusComponent)
	return NpcStatusComponent;
}

#if WITH_EDITOR
void ACoreNpc::ApplyCharacterData(const FTableRowBase* RowData)
{
	if(!GEditor) return;
	if(!RowData)
	{
		UE_LOG(LogTemp, Error, TEXT("RowName Is Not Valid"))
		InitializeStatus(FEnemyNpc());
		return;
	}

	const FEnemyNpc* GetData = static_cast<const FEnemyNpc*>(RowData);
	NMT_CHECKF(GetData)
	if(*GetData == NpcStatusComponent->GetCurrentNpcStatus()) return;
	if(GEditor->GetEditorWorldContext().World())
		InitializeStatus(*GetData);
}
#endif
