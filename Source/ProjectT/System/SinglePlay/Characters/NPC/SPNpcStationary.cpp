// Fill out your copyright notice in the Description page of Project Settings.


#include "SPNpcStationary.h"

#include "Animations/SPNpcAgentAnimInstance.h"
#include "ProjectT/System/Core/Characters/NPC/AI/Controllers/AIControllerBase.h"


ASPNpcStationary::ASPNpcStationary()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ASPNpcStationary::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}

void ASPNpcStationary::BeginPlay()
{
	Super::BeginPlay();
}

void ASPNpcStationary::Respawn()
{
	Super::Respawn();
}

void ASPNpcStationary::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASPNpcStationary::ActivateCharacter(const bool InbEnable)
{
	Super::ActivateCharacter(InbEnable);

	if(InbEnable)
		SetReturnLocationFromDefault();
}

void ASPNpcStationary::ChangeNpcState(const ENpcState InState)
{
	Super::ChangeNpcState(InState);

	if(CurrentState != InState)
	{
		switch(InState)
		{
			case ENpcState::Default:
			{
				ChangeSpeed(ENpcSpeedType::Stop, true);
				AgentAnimInstance->SetNormalState(true);
				AgentAnimInstance->ChangeNpcNormalAnimState(ENpcNormalAnimState::Idle);
				break;
			}
			case ENpcState::Doubt: break;
			case ENpcState::Chase: break;
			case ENpcState::Attack: break;
			case ENpcState::Guard: break;
			case ENpcState::Hit: break;
			case ENpcState::Dead: break;
			case ENpcState::Returning: break;
			default: NMT_LOG("Not Valid ENpcState")
		}
		
		CurrentState = InState;
		GetAIControllerBase()->ChangeNpcStateKey(InState);
		ChangeIndicatorState(InState);
	}
}
