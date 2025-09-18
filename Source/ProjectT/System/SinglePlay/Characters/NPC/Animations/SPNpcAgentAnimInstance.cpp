// Fill out your copyright notice in the Description page of Project Settings.


#include "SPNpcAgentAnimInstance.h"

#include "ProjectT/System/SinglePlay/Characters/NPC/SPNpcAgent.h"

USPNpcAgentAnimInstance::USPNpcAgentAnimInstance() :
	NpcActionMontage(nullptr),
	CurrentNormalState(ENpcNormalAnimState::None),
	PrevNormalState(ENpcNormalAnimState::None),
	CurrentAbnormalState(ENpcAbnormalAnimState::None),
	bWalk(false),
	bRun(false),
	bNormal(true),
	bAction(false)
{
}

void USPNpcAgentAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	OwnerNpcCharacter = Cast<ASPNpcAgent>(TryGetPawnOwner());
}

void USPNpcAgentAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
}

void USPNpcAgentAnimInstance::ChangeNpcAbnormalAnimState(const ENpcAbnormalAnimState InAbnormalState)
{
	bNormal = false;

	if(Montage_IsPlaying(NpcActionMontage))
	{
		if(Montage_GetEndedDelegate(NpcActionMontage)->IsBound())
			Montage_GetEndedDelegate(NpcActionMontage)->Unbind();

		Montage_Stop(0.f);
	}

	switch(InAbnormalState)
	{
		case ENpcAbnormalAnimState::HitBack:
		{
			NMT_LOG("HitBack");
			break;
		}
		case ENpcAbnormalAnimState::JumpBack:
		{
			NMT_LOG("JumpBack");
			break;
		}
		case ENpcAbnormalAnimState::FallDown:
		{
			NMT_LOG("FallDown");
			break;
		}
		case ENpcAbnormalAnimState::Stun:
		{
			NMT_LOG("Stun");
			break;
		}
		case ENpcAbnormalAnimState::DeadStart:
		{
			NMT_LOG("DeadStart");
			break;
		}
		case ENpcAbnormalAnimState::DeadLoop:
		{
			OwnerNpcCharacter->StartDisableNpc();
			NMT_LOG("DeadLoop");
			break;
		}
		case ENpcAbnormalAnimState::GetUp:
		{
			NMT_LOG("GetUp");
			break;
		}
		case ENpcAbnormalAnimState::None: default: NMT_LOG("Not Valid");
	}

	CurrentAbnormalState = InAbnormalState;
}

void USPNpcAgentAnimInstance::ChangeNpcNormalAnimState(const ENpcNormalAnimState InNormalState)
{
	if(!bNormal)
		return;

	switch(InNormalState)
	{
		case ENpcNormalAnimState::Idle:
		{
			bWalk = false;
			bRun = false;
			break;
		}
		case ENpcNormalAnimState::Walk:
		{
			bWalk = true;
			bRun = false;
			break;
		}
		case ENpcNormalAnimState::Run:
		{
			bWalk = false;
			bRun = true;
			break;
		}
		default: NMT_LOG("Not Valid");
	}

	bNormal = true;
	CurrentNormalState = InNormalState;
}

void USPNpcAgentAnimInstance::ResetAnimState()
{
	SetNormalState(true);
	CurrentNormalState = ENpcNormalAnimState::None;
	PrevNormalState = ENpcNormalAnimState::None;
	CurrentAbnormalState = ENpcAbnormalAnimState::None;
}

void USPNpcAgentAnimInstance::SetActionMontage(class UAnimMontage* InActionMontage)
{
	NpcActionMontage = InActionMontage;
}

void USPNpcAgentAnimInstance::SetNormalState(bool InNormal)
{
	bNormal = InNormal;
}

void USPNpcAgentAnimInstance::SetActionState(bool InAction)
{
	bAction = InAction;
}
