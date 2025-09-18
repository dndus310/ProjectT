// Fill out your copyright notice in the Description page of Project Settings.


#include "SPNpcPatrol.h"

#include "Animations/SPNpcAgentAnimInstance.h"
#include "ProjectT/System/Core/Characters/NPC/AI/Controllers/AIControllerBase.h"
#include "ProjectT/System/Core/Common/GlobalUtilsLibrary.h"
#include "ProjectT/System/Core/Components/PathFollowComponent.h"
#include "ProjectT/System/SinglePlay/Components/SPNpcStatusComponent.h"


ASPNpcPatrol::ASPNpcPatrol() :
	PathFollowComponent(nullptr),
	CurrentPatrolType(EPatrolType::Patrol)
{
	PrimaryActorTick.bCanEverTick = true;

	PathFollowComponent = CreateDefaultSubobject<UPathFollowComponent>(TEXT("PathFollowComponent"));
	NMT_CHECKF(PathFollowComponent)
}

void ASPNpcPatrol::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	InitializePatrolInfo();
}

void ASPNpcPatrol::BeginPlay()
{
	Super::BeginPlay();
}

void ASPNpcPatrol::Respawn()
{
	Super::Respawn();
}

void ASPNpcPatrol::ActivateCharacter(const bool InbEnable)
{
	Super::ActivateCharacter(InbEnable);

	if(InbEnable)
	{
		ChangePatrolType();
	}
}

void ASPNpcPatrol::ForgetTarget_Implementation(AActor* InTarget)
{
	switch(CurrentState)
	{
		case ENpcState::Doubt:
		{
			NMT_CHECKF(PathFollowComponent)
			GetAIControllerBase()->UpdateTargetInfo(nullptr);
			ChangePatrolType(true, CurrentPatrolType);
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

void ASPNpcPatrol::ChangeNpcState(const ENpcState InState)
{
	Super::ChangeNpcState(InState);

	if(CurrentState != InState)
	{
		switch(InState)
		{
			case ENpcState::Doubt:
			{
				NMT_CHECKF(PathFollowComponent)
				PathFollowComponent->StopMove();
				break;
			}
			case ENpcState::Returning: break;
			case ENpcState::Default: break;
			case ENpcState::Chase: break;
			case ENpcState::Attack: break;
			case ENpcState::Guard: break;
			case ENpcState::Dead: break;
			case ENpcState::Hit: break;
			default: NMT_LOG("Not Valid ENpcState")
		}
		
		CurrentState = InState;
		GetAIControllerBase()->ChangeNpcStateKey(InState);
		ChangeIndicatorState(InState);
	}
}

void ASPNpcPatrol::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASPNpcPatrol::ApplyDamageEvent_Implementation(AActor* InOccurActor, const FVector& InOriginLocation, const float InDamage, const TArray<FEffect>& InSkillEffects)
{
	Super::ApplyDamageEvent_Implementation(InOccurActor, InOriginLocation, InDamage, InSkillEffects);

	for(FEffect EffectType : InSkillEffects)
	{
		EEffectSubTypes EffectSubType = UGlobalUtilsLibrary::GetStringToEnum<EEffectSubTypes>(EffectType.sEffect);
		switch(EffectSubType)
		{
			case EEffectSubTypes::Surge:
			{
				SetReturnLocationFromDefault();
				break;
			}
			case EEffectSubTypes::Bind:
			{
				GetAIControllerBase()->UpdateNextPatrolType(EPatrolType::None);
				break;
			}
			case EEffectSubTypes::Hitback: break;
			case EEffectSubTypes::Jumpback: break;
			case EEffectSubTypes::Slow: break;
			case EEffectSubTypes::Unrecog: break;
			case EEffectSubTypes::None: break;
			case EEffectSubTypes::Fast: break;
			case EEffectSubTypes::CogImmune: break;
			case EEffectSubTypes::Stun: break;
			case EEffectSubTypes::HitbackImmune: break;
			case EEffectSubTypes::Heal: break;
			default: NMT_LOG("Not Valid");
		}
	}
}

void ASPNpcPatrol::ApplyRecoveryEvent_Implementation(const TArray<FEffect>& InReleaseEffects)
{
	Super::ApplyRecoveryEvent_Implementation(InReleaseEffects);
	
	for(FEffect LocCurEffect : InReleaseEffects)
	{
		EEffectSubTypes LoCurReleaseEffect =  UGlobalUtilsLibrary::GetStringToEnum<EEffectSubTypes>(LocCurEffect.sEffect);
		switch(LoCurReleaseEffect)
		{
			case EEffectSubTypes::Bind:
			{
				if(CurrentState == ENpcState::Default)
					ChangePatrolType(true, CurrentPatrolType);
				break;
			}
			case EEffectSubTypes::Unrecog: break;
			case EEffectSubTypes::Surge: break;
			case EEffectSubTypes::Fast: break;
			case EEffectSubTypes::Slow: break;
			case EEffectSubTypes::Hitback:  break;
			case EEffectSubTypes::Jumpback:  break;
			case EEffectSubTypes::CogImmune:  break;
			case EEffectSubTypes::None: break;
			case EEffectSubTypes::Stun: break;
			case EEffectSubTypes::HitbackImmune: break;
			case EEffectSubTypes::Heal: break;
			default: NMT_LOG("No Release Effect Type")
		}
	}
}

void ASPNpcPatrol::InitializePatrolInfo()
{
	PathFollowComponent->OnSuccessMove.AddUObject(this, &ASPNpcPatrol::ChangePatrolType);

	NMT_CHECKF(NpcStatusComponent)
	int32 MoveProb = static_cast<int32>(NpcStatusComponent->GetStatusNpcMoveProb());
	int32 SearchProb = MoveProb + static_cast<int32>(NpcStatusComponent->GetStatusNpcSearchProb());
	int32 ScoutProb = SearchProb + static_cast<int32>(NpcStatusComponent->GetStatusNpcScoutProb());

	PatrolTypeWeights.Add({EPatrolType::Patrol, MoveProb});
	PatrolTypeWeights.Add({EPatrolType::Search, SearchProb});
	PatrolTypeWeights.Add({EPatrolType::SearchPatrol, ScoutProb});
}

EPatrolType ASPNpcPatrol::SelectNextPatrolType()
{
	EPatrolType ReturnType = EPatrolType::Patrol;
	int32 RandomValue = FMath::RandRange(1, 100);
	
	for(TPair<EPatrolType, int32>& Weight : PatrolTypeWeights)
	{
		if(RandomValue <= Weight.Value)
		{
			ReturnType = Weight.Key;
			break;
		}
	}

	return ReturnType;
}

void ASPNpcPatrol::ChangePatrolType(bool InbForcedAction, EPatrolType InActionType)
{
	if(InbForcedAction)
		CurrentPatrolType = InActionType;
	else
		CurrentPatrolType = SelectNextPatrolType();

	switch(CurrentPatrolType)
	{
		case EPatrolType::Patrol: case EPatrolType::SearchPatrol:
		{
			ChangeSpeed(ENpcSpeedType::Walk);
			AgentAnimInstance->SetNormalState(true);
			AgentAnimInstance->ChangeNpcNormalAnimState(ENpcNormalAnimState::Walk);
			break;
		}
		case EPatrolType::Search:
		{
			ChangeSpeed(ENpcSpeedType::Stop);
			AgentAnimInstance->SetNormalState(true);
			AgentAnimInstance->ChangeNpcNormalAnimState(ENpcNormalAnimState::Idle);
			break;
		}
		case EPatrolType::None:
			break;
		default: NOT_IMPLEMENTED()
	}
	
	GetAIControllerBase()->UpdateNextPatrolType(CurrentPatrolType);
}
