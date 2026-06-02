// Fill out your copyright notice in the Description page of Project Settings.


#include "SPNpcAgent.h"

#include "NavigationSystem.h"
#include "NiagaraComponent.h"
#include "Animations/SPNpcAgentAnimInstance.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "ProjectT/ProjectT.h"
#include "ProjectT/Data/Gen/GenerateEnumDataTableKey.h"
#include "ProjectT/Data/Gen/GenerateEnumEffectSubTypes.h"
#include "ProjectT/Data/Gen/GenerateEnumSkillTypes.h"
#include "ProjectT/Data/Gen/GenerateStructEffect.h"
#include "ProjectT/System/Core/Characters/Player/CorePlayerCharacter.h"
#include "ProjectT/System/Core/Characters/NPC/AI/Controllers/AIControllerBase.h"
#include "ProjectT/System/Core/Characters/NPC/AI/Controllers/AIPerceptionController.h"
#include "ProjectT/System/Core/Characters/NPC/AI/Controllers/AISightController.h"
#include "ProjectT/System/Core/Characters/Skill/CoreSkill.h"
#include "ProjectT/System/Core/Common/AssetUtilsLibrary.h"
#include "ProjectT/System/Core/Common/GlobalUtilsLibrary.h"
#include "ProjectT/System/Core/Components/ObstacleFilterComponent.h"
#include "ProjectT/System/Core/Components/PathFollowComponent.h"
#include "ProjectT/System/Core/Components/VisionConeComponent.h"
#include "ProjectT/System/Core/GameModes/WorldGameMode.h"
#include "ProjectT/System/Core/Managers/CoreCheatManager.h"
#include "ProjectT/System/Core/Managers/PooledActorManager.h"
#include "ProjectT/System/SinglePlay/Components/SPNpcStatusComponent.h"
#include "ProjectT/System/SinglePlay/GameModes/SPGameModeBase.h"
#include "ProjectT/System/SinglePlay/Interfaces/SPObjectProvider.h"
#include "ProjectT/System/SinglePlay/Managers/SPEnemyVisionManager.h"


ASPNpcAgent::ASPNpcAgent() :
	CurrentSpeed(ENpcSpeedType::Walk),
	SkillType(ESkillTypes::None),
	ChaseRangeProfileName(TEXT("ChaseRange")),
	DissolveParameterName(TEXT("Dissolve")),
	SkillCoolTime(0.f),
	DissolveElapsedTime(0.f),
	DissolveDuration(1.5f),
	bRotateWhileAttack(false),
	bSkillCooldownActive(false),
	bDissolving(false),
	CurrentUsingSkill(nullptr),
	AgentAnimInstance(nullptr),
	NpcActionMontage(nullptr)
{
	PrimaryActorTick.bCanEverTick = true;

	AIControllerClass = AAISightController::StaticClass();
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("Enemy"));
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	GetMesh()->SetAnimInstanceClass(USPNpcAgentAnimInstance::StaticClass());
	GetMesh()->SetCustomDepthStencilValue(1);
	WeaponStaticComponent->SetCustomDepthStencilValue(1);
	WeaponStaticComponent->SetCustomDepthStencilValue(1);
	
	HpBarWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("HpBarWidgetComponent"));
	DetectIndicatorWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("DetectIndicatorWidget"));
	SphereChaseRangeComponent = CreateDefaultSubobject<UObstacleFilterComponent>(TEXT("SphereChaseRangeComponent"));
	DissolveFXComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("DissolveFXComponent"));
	VisionLineMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisionLineMeshComponent"));
	NMT_CHECKF(HpBarWidgetComponent && DetectIndicatorWidget && SphereChaseRangeComponent && DissolveFXComponent && VisionLineMeshComponent)
	
	HpBarWidgetComponent->SetupAttachment(WidgetRootComponent);
	HpBarWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);

	DetectIndicatorWidget->SetupAttachment(WidgetRootComponent);
	DetectIndicatorWidget->SetWidgetSpace(EWidgetSpace::Screen);
	
	SphereChaseRangeComponent->SetupAttachment(SubRootComponent);
	SphereChaseRangeComponent->SetCollisionProfileName(ChaseRangeProfileName);
	SphereChaseRangeComponent->SetLineThickness(1.f);

	DissolveFXComponent->SetupAttachment(SubRootComponent);
	DissolveFXComponent->bAutoActivate = false;

	VisionLineMeshComponent->SetupAttachment(SubRootComponent);
	VisionLineMeshComponent->SetRelativeLocation(FVector(0, 0, -GetCapsuleComponent()->GetScaledCapsuleHalfHeight() - 1.f));
	VisionLineMeshComponent->SetHiddenInGame(true);
	VisionLineMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	VisionLineMeshComponent->SetComponentTickEnabled(false);
}

void ASPNpcAgent::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void ASPNpcAgent::BeginPlay()
{
	Super::BeginPlay();

	InitializeMaterials();
}

void ASPNpcAgent::InitializeStatus(const FEnemyNpc InNpcStatus)
{
	Super::InitializeStatus(InNpcStatus);
	
	GetCharacterMovement()->MaxWalkSpeed = InNpcStatus.fNpcMove;

	NMT_CHECKF(VisionConeComponent && NpcStatusComponent);;
	VisionConeComponent->SetupSightSettings(NpcStatusComponent->GetStatusNpcSightLength(), NpcStatusComponent->GetStatusNpcSightAngle());
}

void ASPNpcAgent::Respawn()
{
	Super::Respawn();
	
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	AgentAnimInstance->ChangeNpcNormalAnimState(ENpcNormalAnimState::Idle);
}

void ASPNpcAgent::ExecuteClickedEvent(UPrimitiveComponent* InTouchedComponent, FKey InButtonPressed)
{
	if(InButtonPressed == EKeys::RightMouseButton)
	{
		UWorld* World = UGlobalUtilsLibrary::GetValidWorld(this);
		NMT_CHECKF(World)
		ASPGameModeBase* SPGM = UGlobalUtilsLibrary::GetGameModeChecked<ASPGameModeBase>(World);
		NMT_CHECKF(SPGM)
		USPEnemyVisionManager* EM = ISPObjectProvider::Execute_GetEnemyVisionManager(SPGM);
		NMT_CHECKF(EM)
		EM->RegisterFocused(this);
	}
}

void ASPNpcAgent::ExecuteBeginCursorOverEvent(UPrimitiveComponent* InTouchedComponent)
{
	GetMesh()->SetRenderCustomDepth(true);
	WeaponStaticComponent->SetRenderCustomDepth(true);
	WeaponDynamicComponent->SetRenderCustomDepth(true);
}

void ASPNpcAgent::ExecuteEndCursorOverEvent(UPrimitiveComponent* InTouchedComponent)
{
	GetMesh()->SetRenderCustomDepth(false);
	WeaponStaticComponent->SetRenderCustomDepth(false);
	WeaponDynamicComponent->SetRenderCustomDepth(false);
}

void ASPNpcAgent::ResetStateAfterGameEnd()
{
	Super::ResetStateAfterGameEnd();

	StopSkillActionMontage();
	FinishCurrentSkill();
	FinishCheckInChaseRange();
}

#if WITH_EDITOR
void ASPNpcAgent::ApplyCharacterData(const FTableRowBase* RowData)
{
	Super::ApplyCharacterData(RowData);
}
#endif

void ASPNpcAgent::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(bSkillCooldownActive)
	{
		UpdateSkillCooldown(DeltaTime);
	}
	
	if(bDissolving)
	{
		UpdateDissolve(DeltaTime);
	}

	if(CurrentVisionMode == EVisionMode::Line)
	{
		UpdateVisionLineLength();
	}
}

void ASPNpcAgent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	
	StopSkillActionMontage();

	UWorld* World = UGlobalUtilsLibrary::GetValidWorld(this);
	World->GetTimerManager().ClearTimer(CheckChaseRangeTimerHandle);
	World->GetTimerManager().ClearTimer(JumpBackTimerHandle);
	World->GetTimerManager().ClearTimer(EffectTimerHandle);
}

bool ASPNpcAgent::GetUseSight_Implementation()
{
	return true;
}

bool ASPNpcAgent::GetUseHearing_Implementation()
{
	return true;
}

FSightInfo ASPNpcAgent::GetSightInfo_Implementation()
{
	NMT_CHECKF(NpcStatusComponent)
	bool bNpcDataValid = !NpcStatusComponent->GetCurrentNpcStatus().IsDefault();
	if(!NMT_ENSURE(bNpcDataValid))
		return FSightInfo();

	FSightInfo Info;

	Info.SightRange = NpcStatusComponent->GetStatusNpcSightLength();
	Info.SightAngle = NpcStatusComponent->GetStatusNpcSightAngle();
	Info.LoseSightRange = Info.SightRange + LoseSightOffset;
	Info.SightMaxAge = PerceptionMaxAge;

	return Info;
}

class UBlackboardData* ASPNpcAgent::GetBlackboardData_Implementation()
{
	return Blackboard;
}

class UBehaviorTree* ASPNpcAgent::GetBehaviorTree_Implementation()
{
	return BehaviorTree;
}

void ASPNpcAgent::DetectTargetBySight_Implementation(AActor* InTarget, FAIStimulus InStimulus)
{
	switch(CurrentState)
	{
		case ENpcState::Default:
		{
			ChangeDoubtState(EDoubtState::Increasing);
			ChangeNpcState(ENpcState::Doubt);
			break;
		}
		case ENpcState::Guard: case ENpcState::Returning:
		{
			ChangeNpcState(ENpcState::Chase);
			ApplyStateToQuest(ENpcState::Chase);
			break;
		}
		case ENpcState::Doubt: break;
		case ENpcState::Chase: break;
		case ENpcState::Attack: break;
		case ENpcState::Hit: break;
		case ENpcState::Dead: break;
		default : NMT_LOG("Not Valid Type")
	}
}

void ASPNpcAgent::ForgetTarget_Implementation(AActor* InTarget)
{
	switch(CurrentState)
	{
		case ENpcState::Doubt:
		{
			GetAIControllerBase()->UpdateTargetInfo(nullptr);
			ChangeNpcState(ENpcState::Default);
			break;
		}
		case ENpcState::Default: break;
		case ENpcState::Guard: break;
		case ENpcState::Returning: break;
		case ENpcState::Chase: break;
		case ENpcState::Attack: break;
		case ENpcState::Hit: break;
		case ENpcState::Dead : break;
		default : NMT_LOG("Not Valid Type")
	}
}

FTransform ASPNpcAgent::GetViewTransform_Implementation()
{
	FTransform Transform;
	Transform.SetLocation(SubRootComponent->GetComponentLocation());
	Transform.SetRotation(SubRootComponent->GetComponentRotation().Quaternion());
	Transform.SetScale3D(FVector(1, 1, 1));

	return Transform;
}

void ASPNpcAgent::InitializeProperty()
{
	Super::InitializeProperty();

	AgentAnimInstance = Cast<USPNpcAgentAnimInstance>(GetMesh()->GetAnimInstance());
	NMT_CHECKF(AgentAnimInstance);
	AgentAnimInstance->SetActionMontage(NpcActionMontage);

	NMT_CHECKF(SphereChaseRangeComponent && NpcStatusComponent);
	FObstacleFilterInfo FilterInfo;
	FilterInfo.TraceCollisionChannels.Add(ECC_GameTraceChannel4);	// NOTE : Wall
	FilterInfo.TraceCollisionChannels.Add(ECC_GameTraceChannel6);  // NOTE : Prop
	FilterInfo.SphereRadius = NpcStatusComponent->GetStatusNpcChaseLength();
	FilterInfo.CheckOutOfRange = true;
	SphereChaseRangeComponent->InitializeProperty(FilterInfo);
	SphereChaseRangeComponent->DeactivateFiltering();
	
	SetSkillClass();
}

void ASPNpcAgent::ApplyDamageEvent_Implementation(AActor* InOccurActor, const FVector& InOriginLocation, const float InDamage, const TArray<FEffect>& InSkillEffects)
{
	UCoreCheatManager* CCM = UGlobalUtilsLibrary::GetSharedCoreCheatManager(this);
	float LocFinalDamage = CCM->GetDebugStatus().bDebugPlayerDamage? InDamage + 1000.f : InDamage;
	
	if(NpcStatusComponent->ApplyDamage(LocFinalDamage))
	{
		ChangeNpcState(ENpcState::Dead);
	}
	else
	{
		if(CCM->GetDebugStatus().bDebugEnemyInvincibleStatusEffect) return;
		if(InDamage > 0.f)
		{
			HpBarWidgetComponent->GetUserWidgetObject()->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			GetAIControllerBase()->UpdateTargetInfo(InOccurActor);
		}
		
		for(FEffect EffectType : InSkillEffects)
		{
			const EEffectSubTypes EffectSubType = UGlobalUtilsLibrary::GetStringToEnum<EEffectSubTypes>(EffectType.sEffect);
			if(CheckEffectTime(EffectType))
			{
				switch(EffectSubType)
				{
					case EEffectSubTypes::None:
					{
						ChangeNpcState(ENpcState::Chase);
						break;
					}
					case EEffectSubTypes::Hitback:
					{
						ChangeNpcState(ENpcState::Hit);
						if(!NpcStatusComponent->IsDead())
							AgentAnimInstance->ChangeNpcAbnormalAnimState(ENpcAbnormalAnimState::HitBack);
						break;
					}
					case EEffectSubTypes::Jumpback:
					{
						ChangeNpcState(ENpcState::Hit);
						JumpBack(InOriginLocation);
						break;
					}
					case EEffectSubTypes::Slow:
					{
						if(InDamage > 0.f && CurrentState != ENpcState::Attack)
						{
							SetReturnLocationFromDefault();
							ChangeNpcState(ENpcState::Chase);
						}
						
						StatusSlow(EffectType.fEffectValue);
						break;
					}
					case EEffectSubTypes::Fast:
					{
						if(InDamage > 0.f && CurrentState != ENpcState::Attack)
						{
							SetReturnLocationFromDefault();
							ChangeNpcState(ENpcState::Chase);
						}
						
						StatusFast(EffectType.fEffectValue);
						break;
					}
					case EEffectSubTypes::Bind:
					{
						if(InDamage > 0.f && CurrentState != ENpcState::Attack)
						{
							SetReturnLocationFromDefault();
							ChangeNpcState(ENpcState::Chase);
						}
					
						ChangeSpeed(ENpcSpeedType::Stop, true);
						break;
					}
					case EEffectSubTypes::Unrecog:
					{
						SetUnDetectTarget(true);
						break;
					}
					case EEffectSubTypes::CogImmune: NOT_IMPLEMENTED_MSG("EEffectSubTypes::CogImmune"); break;
					case EEffectSubTypes::Stun: NOT_IMPLEMENTED_MSG("EEffectSubTypes::Stun"); break;
					case EEffectSubTypes::Surge: NOT_IMPLEMENTED_MSG("EEffectSubTypes::Surge"); break;
					case EEffectSubTypes::HitbackImmune: NOT_IMPLEMENTED_MSG("EEffectSubTypes::HitbackImmune"); break;
					case EEffectSubTypes::Heal: NOT_IMPLEMENTED_MSG("EEffectSubTypes::Heal"); break;
					default: NMT_LOG("Not Valid");
				}
				AddEffectTimeMap(EffectType);
			}
		}
	}
}

void ASPNpcAgent::ApplyRecoveryEvent_Implementation(const TArray<FEffect>& InReleaseEffects)
{
	for(FEffect LocCurEffect : InReleaseEffects)
	{
		EEffectSubTypes LoCurReleaseEffect =  UGlobalUtilsLibrary::GetStringToEnum<EEffectSubTypes>(LocCurEffect.sEffect);
		EffectTimeMap.Remove(LoCurReleaseEffect);
		if(EffectTimeMap.IsEmpty())
		{
			GetWorld()->GetTimerManager().ClearTimer(EffectTimerHandle);
		}
		switch(LoCurReleaseEffect)
		{
			case EEffectSubTypes::Unrecog:
			{
				SetUnDetectTarget(false);
				break;
			}
			case EEffectSubTypes::Surge:
			{
				if(CurrentState == ENpcState::Default)
				{
					ChangeNpcState(ENpcState::Returning);				
				}
				break;
			}	
			case EEffectSubTypes::Bind:
			{
				if(CurrentState == ENpcState::Returning || CurrentState == ENpcState::Chase || CurrentState == ENpcState::Attack)
				{
					ChangeSpeed(ENpcSpeedType::Run);
				}
				else
				{
					ChangeSpeed(ENpcSpeedType::Walk);
				}		
				break;
			}
			case EEffectSubTypes::Fast:
			{
				StatusFast(LocCurEffect.fEffectValue, true);
				break;
			}
			case EEffectSubTypes::Slow:
			{
				StatusSlow(LocCurEffect.fEffectValue, true);
				break;
			}
			case EEffectSubTypes::Hitback: NOT_IMPLEMENTED_MSG("Release EEffectSubTypes::Hitback");	break;
			case EEffectSubTypes::Jumpback: NOT_IMPLEMENTED_MSG("Release EEffectSubTypes::Jumpback"); break;
			case EEffectSubTypes::CogImmune: NOT_IMPLEMENTED_MSG("Release EEffectSubTypes::CogImmune");	break;
			case EEffectSubTypes::None:	break;
			case EEffectSubTypes::Stun:	break;
			case EEffectSubTypes::HitbackImmune: break;
			case EEffectSubTypes::Heal:	break;
			default: NMT_LOG("No Release Effect Type")
		}
	}
}

void ASPNpcAgent::StartDisableNpc()
{
	Super::StartDisableNpc();

	DissolveFXComponent->Activate(true);
	bDissolving = true;
	NotifyVisionDetection(false);
	ChangeSpeed(ENpcSpeedType::Stop, true);
	ClearSkillCoolTime();
	FinishCurrentSkill();
	FinishCheckInChaseRange();
}

void ASPNpcAgent::CompleteDisableNpc()
{
	Super::CompleteDisableNpc();

	AgentAnimInstance->ResetAnimState();
	for(UMeshComponent* MeshComponent : AllMeshComponents)
	{
		if(MeshComponent == nullptr)
			continue;

		MeshComponent->SetScalarParameterValueOnMaterials(DissolveParameterName, 1.f);
	}
}

class USPNpcAgentAnimInstance* ASPNpcAgent::GetAgentAnimInstance() const
{
	NMT_CHECKF(AgentAnimInstance);
	return AgentAnimInstance;
}

void ASPNpcAgent::ChangeNpcState(const ENpcState InState)
{
	PrevState = CurrentState;

	if(CurrentState != InState)
	{
		switch(InState)
		{
			case ENpcState::Default:
			{
				bCanRotateHeadToTarget = true;
				if(CurrentDoubtState == EDoubtState::Increasing)
					ChangeDoubtState(EDoubtState::Decreasing);
				break;
			}
			case ENpcState::Doubt:
			{
				NotifyVisionDetection(true);
				AgentAnimInstance->SetNormalState(true);
				AgentAnimInstance->ChangeNpcNormalAnimState(ENpcNormalAnimState::Idle);
				break;
			}
			case ENpcState::Chase: 
			{
				if(GetAIControllerBase()->GetTargetActor())
					StartCheckInChaseRange();

				ChangeDoubtState(EDoubtState::None);
				NotifyVisionDetection(false);
				break;
			}
			case ENpcState::Attack:
			{
				if(GetAIControllerBase()->GetTargetActor())
					StartCheckInChaseRange();
				
				NotifyVisionDetection(false);
				break;
			}
			case ENpcState::Guard:
			{
				bCanTriggerAlert = true;
				ChangeSpeed(ENpcSpeedType::Stop);
				AgentAnimInstance->SetNormalState(true);
				AgentAnimInstance->ChangeNpcNormalAnimState(ENpcNormalAnimState::Idle);
				break;
			}
			case ENpcState::Hit:
			{
				SetReturnLocationFromDefault();
				StopSkillActionMontage();
				ClearSkillCoolTime();
				GetAIControllerBase()->SetEnableAttack(true);
				FinishCurrentSkill();
				ChangeSpeed(ENpcSpeedType::Stop, true);
				SetVisionMode(EVisionMode::None);
				break;
			}
			case ENpcState::Dead:
			{
				ChangeSpeed(ENpcSpeedType::Stop, true);
				AgentAnimInstance->ChangeNpcAbnormalAnimState(ENpcAbnormalAnimState::DeadStart);
				ApplyStateToQuest(ENpcState::Dead);
				HpBarWidgetComponent->GetUserWidgetObject()->SetVisibility(ESlateVisibility::Collapsed);
				break;
			}
			case ENpcState::Returning:
			{
				break;
			}
			default: NMT_LOG("Not Valid ENpcState")
		}
	}
}

void ASPNpcAgent::ActivateCharacter(const bool InbEnable)
{
	Super::ActivateCharacter(InbEnable);
	
	DetectIndicatorWidget->SetVisibility(InbEnable);

	if(InbEnable)
		GetAIControllerBase()->SetCanRotateWhileAttack(bRotateWhileAttack);
}

void ASPNpcAgent::SetDetectTargetFromNoise(AActor* InTarget)
{
	Super::SetDetectTargetFromNoise(InTarget);
}

void ASPNpcAgent::SetDetectLocationFromNoise(const FVector& InTargetLocation)
{
	Super::SetDetectLocationFromNoise(InTargetLocation);
}

void ASPNpcAgent::ChangeSpeed(const ENpcSpeedType InSpeed, const bool InbStopImmediately)
{
	float Speed = 0.f;
	switch(InSpeed)
	{
		case ENpcSpeedType::Walk:
		{
			Speed = NpcStatusComponent->GetStatusNpcMove();
			break;
		}
		case ENpcSpeedType::Run:
		{
			Speed = NpcStatusComponent->GetStatusNpcRun();
			break;
		}
		case ENpcSpeedType::Stop:
		{
			if(InbStopImmediately)
				GetCharacterMovement()->StopMovementImmediately();

			if(UPathFollowComponent* PathFollowComponent = FindComponentByClass<UPathFollowComponent>())
				PathFollowComponent->StopMove();
			break;
		}
		default: NMT_LOG("Not Valid Type")
	}
	
	CurrentSpeed = InSpeed;
	EffectTimeMap.Find(EEffectSubTypes::Bind) ? GetCharacterMovement()->MaxWalkSpeed = 0.f : GetCharacterMovement()->MaxWalkSpeed = Speed;
}

class ACoreSkill* ASPNpcAgent::GetCurrentSkill() const
{
	return CurrentUsingSkill;
}

void ASPNpcAgent::GetSkillClass(TSubclassOf<AActor>& OutSkillClass) const
{
	OutSkillClass = NpcSkillClass;
}

void ASPNpcAgent::UpdateDissolve(const float InDeltaSeconds)
{
	DissolveElapsedTime += InDeltaSeconds;
	float Alpha = FMath::Clamp(1.f - (DissolveElapsedTime / DissolveDuration), 0.f, 1.f);

	if(FMath::IsNearlyZero(Alpha))
	{
		bDissolving = false;
		DissolveElapsedTime = 0.f;
		CompleteDisableNpc();
	}
	else
	{
		for(UMeshComponent* MeshComponent : AllMeshComponents)
		{
			if(MeshComponent == nullptr)
				continue;

			MeshComponent->SetScalarParameterValueOnMaterials(DissolveParameterName, Alpha);
		}
	}
}

void ASPNpcAgent::NotifyVisionDetection(const bool InbDetect)
{
	UWorld* World = UGlobalUtilsLibrary::GetValidWorld(this);
	NMT_CHECKF(World)
	ASPGameModeBase* SPGM = UGlobalUtilsLibrary::GetGameModeChecked<ASPGameModeBase>(World);
	NMT_CHECKF(SPGM)
	USPEnemyVisionManager* EM = ISPObjectProvider::Execute_GetEnemyVisionManager(SPGM);
	if(EM)
	{
		InbDetect ? EM->RegisterDetection(this) : EM->UnregisterDetection(this);
	}
}

void ASPNpcAgent::ShowVisual_Implementation()
{
	
}

void ASPNpcAgent::HideVisual_Implementation()
{
}

void ASPNpcAgent::UpdateVisionLineLength()
{
	AActor* Target = GetAIControllerBase()->GetTargetActor();
	if(!Target)
		return;
	
	NMT_CHECKF(VisionLineMeshComponent)
	FVector Start = VisionLineMeshComponent->GetComponentLocation();
	FVector End = Target->GetActorLocation();
	float Distance = FVector::Dist2D(Start, End);
	float ScaleX = Distance / 100.f;
	FRotator LookRotator = UKismetMathLibrary::FindLookAtRotation(Start, End);
	
	VisionLineMeshComponent->SetRelativeScale3D(FVector(ScaleX, 1.f, 1.f));
	VisionLineMeshComponent->SetWorldRotation(LookRotator);
}

void ASPNpcAgent::PlayActionMontage(FName InSectionName, FOnMontageBlendingOutStarted InEndedFunction)
{
	NMT_CHECKF(AgentAnimInstance)
	if(!NMT_ENSURE(NpcActionMontage))
		return;
	
	AgentAnimInstance->SetActionState(true);
	
	AgentAnimInstance->Montage_Play(NpcActionMontage, 1, EMontagePlayReturnType::MontageLength, 0, true);
	if(AgentAnimInstance->GetInstanceForMontage(NpcActionMontage))
	{
		FAnimMontageInstance* MontageInstance = AgentAnimInstance->GetInstanceForMontage(NpcActionMontage);
		if(MontageInstance && MontageInstance->OnMontageBlendingOutStarted.IsBound())
		{
			MontageInstance->OnMontageBlendingOutStarted.Unbind();
		}		
	}
	AgentAnimInstance->Montage_SetBlendingOutDelegate(InEndedFunction);
	AgentAnimInstance->Montage_JumpToSection(InSectionName);
}

void ASPNpcAgent::TryAttack()
{
	if(!IsFinishSkillCoolDown())
	{
		NMT_LOGF("Skill CoolDown.. : %f", SkillCoolTime)
		return;
	}

	UWorld* World = UGlobalUtilsLibrary::GetValidWorld(this);
	AWorldGameMode* LocGameMode = UGlobalUtilsLibrary::GetGameModeChecked<AWorldGameMode>(World);
	UPooledActorManager* LocPoolManager = IWorldObjectProvider::Execute_GetPooledActorManager(LocGameMode);
	if(!LocPoolManager)
		return;

	AgentAnimInstance->ChangeNpcNormalAnimState(ENpcNormalAnimState::Idle);
	ChangeSpeed(ENpcSpeedType::Stop, true);
	CurrentUsingSkill = Cast<ACoreSkill>(LocPoolManager->Obtain(NpcSkillClass));
	if(CurrentUsingSkill)
	{
		CurrentUsingSkill->SetSkillOwner(this);
		StartAttack();
		GetAIControllerBase()->SetEnableAttack(false);
		SetHeadRotationEnable(false);
		return;
	}
	
	NMT_MSG_ENSURE(0, "Fail to obtain Npc Skill");
}

void ASPNpcAgent::StartAttack()
{
	FSkill SkillData = CurrentUsingSkill->GetSkillData();
	ActionMontageSectionName = SkillData.sSkillName;
	
	switch(SkillType)
	{
		case ESkillTypes::Melee:
		{
			CurrentUsingSkill->StartSkill();
			SetSkillCoolTime(SkillData.fCooldown);
			PlayActionMontage(FName(*ActionMontageSectionName), FOnMontageBlendingOutStarted::CreateWeakLambda(this, [this](UAnimMontage* Montage, bool Interrupted)
			{
				TryFinishAttack();
			}));
		}
			break;
		case ESkillTypes::Direct:
		{
			if(PrevState == ENpcState::Attack)
			{
				ContinueAttack();
				return;
			}

			CurrentUsingSkill->StartSkill();
			ActionMontageSectionName.Append("Draw");
			PlayActionMontage(FName(*ActionMontageSectionName), FOnMontageBlendingOutStarted::CreateWeakLambda(this, [this](UAnimMontage* Montage, bool Interrupted)
			{
				ContinueAttack();
			}));
		}
			break;
		case ESkillTypes::None: NOT_IMPLEMENTED_MSG("StartAttack::None")
			break;
		case ESkillTypes::Buff: NOT_IMPLEMENTED_MSG("StartAttack::None")
			break;
		case ESkillTypes::Indirect: NOT_IMPLEMENTED_MSG("StartAttack::None")
			break;
		default : NMT_LOG("Not Valid Type")
	}
}

void ASPNpcAgent::ContinueAttack()
{
	if(CurrentUsingSkill == nullptr) return;
	FSkill SkillData = CurrentUsingSkill->GetSkillData();
	ActionMontageSectionName = SkillData.sSkillName;

	switch(SkillType)
	{
		case ESkillTypes::Direct:
		{
			CurrentUsingSkill->ActivateSkill();
			SetSkillCoolTime(SkillData.fCooldown);
			ActionMontageSectionName.Append("Shoot");
			PlayActionMontage(FName(*ActionMontageSectionName));
		}
			break;
		case ESkillTypes::Melee: NOT_IMPLEMENTED_MSG("ContinueAttack::Melee")
			break;
		case ESkillTypes::None: NOT_IMPLEMENTED_MSG("ContinueAttack::None")
			break;
		case ESkillTypes::Buff: NOT_IMPLEMENTED_MSG("ContinueAttack::None")
			break;
		case ESkillTypes::Indirect: NOT_IMPLEMENTED_MSG("ContinueAttack::None")
			break;
		default : NMT_LOG("Not Valid Type")
	}
}

void ASPNpcAgent::TryFinishAttack()
{
	NMT_LOG("TryFinishAttack")
	switch(SkillType)
	{
		case ESkillTypes::Melee:
		{
			GetAIControllerBase()->SetEnableAttack(true);
			FinishCurrentSkill();
			ShouldContinueAttack() ? ChangeNpcState(ENpcState::Attack) : ChangeNpcState(ENpcState::Chase);
		}
			break;
		case ESkillTypes::Direct:
		{
			if(CurrentUsingSkill == nullptr)
			{
				GetAIControllerBase()->SetEnableAttack(true);
				return;
			}
			
			FSkill SkillData = CurrentUsingSkill->GetSkillData();
			ActionMontageSectionName = SkillData.sSkillName;
	
			if(ShouldContinueAttack())
			{
				NMT_LOG("Attack Again")
				GetAIControllerBase()->SetEnableAttack(true);
				FinishCurrentSkill();
				ChangeNpcState(ENpcState::Attack);
			}
			else
			{
				ActionMontageSectionName.Append("Undraw");
				PlayActionMontage(FName(*ActionMontageSectionName), FOnMontageBlendingOutStarted::CreateWeakLambda(this, [this](UAnimMontage* Montage, bool Interrupted)
				{
					FinishAttack();
				}));
			}
		}
			break;
		case ESkillTypes::None: NOT_IMPLEMENTED_MSG("ESkillTypes::None")
			break;
		case ESkillTypes::Buff: NOT_IMPLEMENTED_MSG("ESkillTypes::None")
			break;
		case ESkillTypes::Indirect: NOT_IMPLEMENTED_MSG("ESkillTypes::None")
			break;
		default : NMT_LOG("Not Valid Type")
	}
}

void ASPNpcAgent::FinishAttack()
{
	FinishCurrentSkill();

	if(!GetController())
		return;
	
	GetAIControllerBase()->SetEnableAttack(true);
	if(!ShouldContinueAttack())
		ChangeNpcState(ENpcState::Chase);
}

void ASPNpcAgent::SetUnDetectTarget(const bool InbUnDetect)
{
	Super::SetUnDetectTarget(InbUnDetect);

	GetAIPerceptionController()->SetUnDetect(bUnDetect);
	if(bUnDetect)
	{
		if(CurrentVisionMode == EVisionMode::Cone)
		{
			UWorld* World = UGlobalUtilsLibrary::GetValidWorld(this);
			NMT_CHECKF(World)
			ASPGameModeBase* SPGM = UGlobalUtilsLibrary::GetGameModeChecked<ASPGameModeBase>(World);
			NMT_CHECKF(SPGM)
			USPEnemyVisionManager* EM = ISPObjectProvider::Execute_GetEnemyVisionManager(SPGM);
			NMT_CHECKF(EM)
			EM->IsFocusedNpc(this) ? EM->UnregisterFocused() : EM->UnregisterDetection(this);
		}
	}
	else
	{
		GetAIPerceptionController()->ForceUpdatePerception();	
	}
}

void ASPNpcAgent::SetVisionMode(EVisionMode InVisionMode)
{
	CurrentVisionMode = InVisionMode;
	NMT_CHECKF(VisionConeComponent && VisionLineMeshComponent)
	
	switch(InVisionMode)
	{
		case EVisionMode::None:
		{
			VisionConeComponent->ToggleDraw(false);
			VisionLineMeshComponent->SetHiddenInGame(true);
			VisionLineMeshComponent->SetComponentTickEnabled(false);
			break;
		}
		case EVisionMode::Cone:
		{
			VisionConeComponent->ToggleDraw(true);
			VisionLineMeshComponent->SetHiddenInGame(true);
			VisionLineMeshComponent->SetComponentTickEnabled(false);
			break;
		}
		case EVisionMode::Line:
		{
			VisionConeComponent->ToggleDraw(false);
			VisionLineMeshComponent->SetComponentTickEnabled(true);
			VisionLineMeshComponent->SetHiddenInGame(false);
			break;
		}
	}
}

void ASPNpcAgent::ChangeDoubtState(const EDoubtState InDoubtState)
{
	Super::ChangeDoubtState(InDoubtState);
	
	switch(InDoubtState)
	{
		case EDoubtState::None:
		{
			NotifyVisionDetection(false);
			break;
		}
		case EDoubtState::Increasing: case EDoubtState::Decreasing: break;
		default: NOT_IMPLEMENTED();
	}
}

void ASPNpcAgent::InitializeMaterials()
{
	TArray<UMeshComponent*> MeshComponents;
	GetComponents<UMeshComponent>(MeshComponents);
	
	for(UMeshComponent* MeshComp : MeshComponents)
	{
		AllMeshComponents.Add(MeshComp);
	}
}

void ASPNpcAgent::StatusSlow(float InSlowValue, bool InbRecovery)
{
	float LocBeforeValue = EffectTimeMap.Find(EEffectSubTypes::Slow)? LocBeforeValue = EffectTimeMap.Find(EEffectSubTypes::Slow)->EffectValue : LocBeforeValue = 0.f;  
	if(LocBeforeValue == InSlowValue) return;
	if(!InbRecovery) InSlowValue = InSlowValue * -1.f;
	float LocSlowValue = InSlowValue - LocBeforeValue;
	NpcStatusComponent->AddStatusNpcMove(LocSlowValue);
	NpcStatusComponent->AddStatusNpcRun(LocSlowValue);

	if(CurrentSpeed == ENpcSpeedType::Walk)
		GetCharacterMovement()->MaxWalkSpeed = GetNpcStatusComponent()->GetCurrentNpcStatus().fNpcMove;
	else if(CurrentSpeed == ENpcSpeedType::Run)
		GetCharacterMovement()->MaxWalkSpeed = GetNpcStatusComponent()->GetCurrentNpcStatus().fNpcRun;
}

void ASPNpcAgent::StatusFast(float InFastValue, bool InbRecovery)
{
	float LocBeforeValue = EffectTimeMap.Find(EEffectSubTypes::Fast)? LocBeforeValue = EffectTimeMap.Find(EEffectSubTypes::Fast)->EffectValue : LocBeforeValue = 0.f;  
	if(LocBeforeValue == InFastValue) return;
	if(InbRecovery) InFastValue = InFastValue * -1.f; 
	float LocFastValue = InFastValue - LocBeforeValue;
	NpcStatusComponent->AddStatusNpcMove(LocFastValue);
	NpcStatusComponent->AddStatusNpcRun(LocFastValue);

	if(CurrentSpeed == ENpcSpeedType::Walk)
		GetCharacterMovement()->MaxWalkSpeed = GetNpcStatusComponent()->GetCurrentNpcStatus().fNpcMove;
	else if(CurrentSpeed == ENpcSpeedType::Run)
		GetCharacterMovement()->MaxWalkSpeed = GetNpcStatusComponent()->GetCurrentNpcStatus().fNpcRun;
}

void ASPNpcAgent::JumpBack(const FVector& InTargetLocation)
{
	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), InTargetLocation);
	AgentAnimInstance->ChangeNpcAbnormalAnimState(ENpcAbnormalAnimState::JumpBack);
	float JumpPower = 100.f;
	FVector JumpDir = (GetActorLocation() - InTargetLocation).GetSafeNormal();
	FVector JumpBackVector = JumpPower * JumpDir;
	FVector LaunchVelocity;

	SetActorRotation(LookAtRotation);
	UGlobalUtilsLibrary::SuggestProjectileVelocityAtLocation(GetWorld(), LaunchVelocity, GetActorLocation(),
	                                                         GetActorLocation() + JumpBackVector, 40.f, 0.f, 1.f);
	LaunchCharacter(LaunchVelocity, true, true);

	UWorld* World = UGlobalUtilsLibrary::GetValidWorld(this);
	if(!World->GetTimerManager().IsTimerActive(JumpBackTimerHandle))
	{
		World->GetTimerManager().SetTimer(JumpBackTimerHandle, this, &ASPNpcAgent::CheckFallingFromJumpBack, World->GetDeltaSeconds(), true, 0.2f);
	}
}

void ASPNpcAgent::CheckFallingFromJumpBack()
{
	if(!GetMovementComponent()->IsFalling() && GetWorld()->GetTimerManager().IsTimerActive(JumpBackTimerHandle))
	{
		GetWorld()->GetTimerManager().ClearTimer(JumpBackTimerHandle);
		JumpBackTimerHandle.Invalidate();
		AgentAnimInstance->ChangeNpcAbnormalAnimState(ENpcAbnormalAnimState::FallDown);
		//	RelocateOnNavMesh();
	}
}

bool ASPNpcAgent::ShouldContinueAttack() const
{
	AActor* Target = GetAIControllerBase()->GetTargetActor();
	if(Target && IsTargetInAttackRange(Target))
	{
		if(SkillType == ESkillTypes::Direct)
		{
			NMT_CHECKF(NpcStatusComponent)
			FCollisionObjectQueryParams ObjectQueryParams;
			ObjectQueryParams.AddObjectTypesToQuery(ECC_GameTraceChannel4);		// NOTE : Wall
			ObjectQueryParams.AddObjectTypesToQuery(ECC_GameTraceChannel6);		// NOTE : Prop
			FHitResult HitResult;
			FVector EndTraceLocation = GetActorLocation() + GetActorForwardVector() * NpcStatusComponent->GetStatusNpcAttackLength();
			GetWorld()->LineTraceSingleByObjectType(HitResult, GetActorLocation(), EndTraceLocation, ObjectQueryParams, FCollisionQueryParams());
			return !HitResult.bBlockingHit;
		}

		return true;
	}

	return false;
}

void ASPNpcAgent::DetermineNextStateAfterAbnormal()
{
	ENpcAbnormalAnimState AbnormalAnimState = AgentAnimInstance->GetNpcAbnormalAnimState();

	switch(AbnormalAnimState)
	{
		case ENpcAbnormalAnimState::HitBack:
		{
			if(!NpcStatusComponent->IsDead())
			{
				AgentAnimInstance->SetNormalState(true);
				ChangeNpcState(ENpcState::Chase);
			}
			break;
		}
		case ENpcAbnormalAnimState::FallDown:
		{
			if(!NpcStatusComponent->IsDead())
			{
				AgentAnimInstance->ChangeNpcAbnormalAnimState(ENpcAbnormalAnimState::GetUp);
			}
			else
			{
				AgentAnimInstance->ChangeNpcAbnormalAnimState(ENpcAbnormalAnimState::DeadLoop);
				ChangeNpcState(ENpcState::Dead);
			}
			break;
		}
		case ENpcAbnormalAnimState::GetUp:
		{
			AgentAnimInstance->SetNormalState(true);
			AActor* Target = GetAIControllerBase()->GetTargetActor();
			if(Target)
			{
				IsTargetInAttackRange(Target) ? ChangeNpcState(ENpcState::Attack) : ChangeNpcState(ENpcState::Chase);
			}
			else
			{
				ChangeNpcState(ENpcState::Chase);
			}
			
			break;
		}
		case ENpcAbnormalAnimState::DeadStart:
		{
			AgentAnimInstance->ChangeNpcAbnormalAnimState(ENpcAbnormalAnimState::DeadLoop);
			GetMesh()->bPauseAnims = true;
			break;
		}
		case ENpcAbnormalAnimState::DeadLoop: case ENpcAbnormalAnimState::JumpBack: case ENpcAbnormalAnimState::Stun: NOT_IMPLEMENTED()
			break;
		default: NMT_LOG("Not Valid")
	}
}

void ASPNpcAgent::SetCurrentUsingSkill(class ACoreSkill* InSkill)
{
	CurrentUsingSkill = InSkill;
}

void ASPNpcAgent:: FinishCurrentSkill()
{
	if(CurrentUsingSkill)
	{
		NMT_LOG("FinishCurrentSkill")
		CurrentUsingSkill->FinishSkill();
		CurrentUsingSkill = nullptr;
	}
}

void ASPNpcAgent::SetSkillCoolTime(const float InCoolTime)
{
	SkillCoolTime = InCoolTime;
	bSkillCooldownActive = true;
}

class AAIPerceptionController* ASPNpcAgent::GetAIPerceptionController() const
{
	AAIPerceptionController* AIC = Cast<AAIPerceptionController>(GetController());
	NMT_CHECKF(AIC)

	return AIC;
}

bool ASPNpcAgent::IsFinishSkillCoolDown() const
{
	return SkillCoolTime <= 0.f;
}

bool ASPNpcAgent::IsTargetInAttackRange(const AActor* InTarget) const
{
	NMT_CHECKF(NpcStatusComponent)
	return GetHorizontalDistanceTo(InTarget) <= NpcStatusComponent->GetStatusNpcAttackLength();
}

void ASPNpcAgent::StartCheckInChaseRange()
{
	NMT_CHECKF(SphereChaseRangeComponent)
	if(!SphereChaseRangeComponent->OnExitedSphereRange.IsBound())
	{
		SphereChaseRangeComponent->ActivateFiltering();
		SphereChaseRangeComponent->OnEnterSphereRange.AddWeakLambda(this, [this](AActor* Target)
		{
			GetAIControllerBase()->UpdateTargetInfo(Target);
		});
		SphereChaseRangeComponent->OnExitedSphereRange.AddWeakLambda(this, [this]()
		{
			GetAIControllerBase()->UpdateTargetInfo(nullptr);
		});
	}

	UWorld* World = UGlobalUtilsLibrary::GetValidWorld(this);
	if(!World->GetTimerManager().IsTimerActive(CheckChaseRangeTimerHandle))
		World->GetTimerManager().SetTimer(CheckChaseRangeTimerHandle, this, &ASPNpcAgent::CheckTargetInChaseRange, World->GetDeltaSeconds(), true);
}

void ASPNpcAgent::StopSkillActionMontage()
{
	NMT_CHECKF(AgentAnimInstance)
	if(!NMT_ENSURE(NpcActionMontage))
		return;

	AgentAnimInstance->SetActionState(false);
	if(AgentAnimInstance->GetInstanceForMontage(NpcActionMontage))
	{
		FAnimMontageInstance* MontageInstance = AgentAnimInstance->GetInstanceForMontage(NpcActionMontage);
		if(MontageInstance && MontageInstance->OnMontageBlendingOutStarted.IsBound())
		{
			MontageInstance->OnMontageBlendingOutStarted.Unbind();
		}		
	}
	AgentAnimInstance->Montage_Stop(0.f, NpcActionMontage);
}

void ASPNpcAgent::SetSkillClass()
{
	UDataTable* SkillDataTable = UAssetUtilsLibrary::LoadDataTableByEnumKey(EDataTableKey::Skill);
	if(!NMT_ENSURE(SkillDataTable))
		return;

	NMT_CHECKF(NpcStatusComponent)
	FString SkillRowName = FString::FromInt(NpcStatusComponent->GetStatusNpcAttackId());
	FSkill* SkillDataRow = SkillDataTable->FindRow<FSkill>(*SkillRowName, TEXT("Not Found FSkill"));
	
	if(!NMT_ENSURE(SkillDataRow))
		return;
		
	FString TempPath = SkillDataRow->sSkillBpPath;
	if(!CHECK_FREE(TempPath))
	{
		FString ClassPath = UAssetUtilsLibrary::AssetReferencePathToClassPath(TempPath);
		UClass* LoadedClass = StaticLoadClass(UObject::StaticClass(), nullptr, *ClassPath);
		NMT_CHECKF(LoadedClass)
		NpcSkillClass = TSubclassOf<ACoreSkill>(LoadedClass);
		SkillType = UGlobalUtilsLibrary::GetStringToEnum<ESkillTypes>(*SkillDataRow->sSkillType);
		bRotateWhileAttack = SkillType == ESkillTypes::Melee;
	}
}

void ASPNpcAgent::ClearSkillCoolTime()
{
	bSkillCooldownActive = false;
	SkillCoolTime = 0.f;
}

void ASPNpcAgent::UpdateSkillCooldown(const float InDeltaSeconds)
{
	if(SkillCoolTime < 0.f)
	{
		ClearSkillCoolTime();
		if(SkillType == ESkillTypes::Direct)
			TryFinishAttack();
	}
	
	SkillCoolTime -= InDeltaSeconds;
}

void ASPNpcAgent::CheckTargetInChaseRange()
{
	if(SphereChaseRangeComponent)
	{
		AActor* TargetActor = GetAIControllerBase()->GetTargetActor();
		if(TargetActor == nullptr)
			return;

		TArray<AActor*> DetectedActor = SphereChaseRangeComponent->GetNonBlockedActors(TargetActor->GetActorLocation(), GetActorLocation());
		ACorePlayerCharacter* DetectedPlayer = nullptr;
		
		for (AActor* Actor : DetectedActor)
		{
			if(ACorePlayerCharacter* Player = Cast<ACorePlayerCharacter>(Actor))
				DetectedPlayer = Player;
		}
			
		if(DetectedPlayer == nullptr)
		{
			NMT_LOG("DetectedPlayer is empty. Player is behind wall")
			FinishCheckInChaseRange();
		}
	}
}

void ASPNpcAgent::FinishCheckInChaseRange()
{
	if(GetWorld()->GetTimerManager().IsTimerActive(CheckChaseRangeTimerHandle) && SphereChaseRangeComponent)
	{
		NMT_LOG("FinishCheckInChaseRange")
		GetWorld()->GetTimerManager().ClearTimer(CheckChaseRangeTimerHandle);
		CheckChaseRangeTimerHandle.Invalidate();
		GetAIControllerBase()->UpdateTargetInfo(nullptr);
		SphereChaseRangeComponent->DeactivateFiltering();
		SphereChaseRangeComponent->OnExitedSphereRange.Clear();

		if(CurrentState != ENpcState::Attack && CurrentState != ENpcState::Hit)
		{
			ChangeNpcState(ENpcState::Chase);
		}
	}
}

void ASPNpcAgent::RelocateOnNavMesh()
{
	UWorld* World = UGlobalUtilsLibrary::GetValidWorld(this);
	NMT_CHECKF(World);

	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(World);
	NMT_CHECKF(NavSys);

	float CapsuleRadius, CapsuleHalfHeight;
	GetCapsuleComponent()->GetScaledCapsuleSize(CapsuleRadius, CapsuleHalfHeight);
	FVector Extent(CapsuleRadius, CapsuleRadius, CapsuleHalfHeight);
	FNavLocation OutLocation;

	bool bOnNavMesh = NavSys->ProjectPointToNavigation(GetActorLocation(), OutLocation, Extent);
	if(bOnNavMesh)
	{
		NMT_LOG("On Nav Mesh")
	}
	else
	{
		NMT_LOG("NOT On Nav Mesh")
		Extent += FVector(50.0, 50.0, 0.0);
		if(NavSys->ProjectPointToNavigation(GetActorLocation(), OutLocation, Extent))
		{
			SetActorLocation(OutLocation);
		}
	}
}
