// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacterAnimInstance.h"

#include "ProjectT/ProjectT.h"
#include "ProjectT/System/Core/Characters/Player/CorePlayerCharacter.h"

UPlayerCharacterAnimInstance::UPlayerCharacterAnimInstance() :
	OwnerPlayerCharacter(nullptr),
	bPlayerInputable(true),
	bIdle(true),
	bRest(false),
	bWalk(false),
	bRun(false),
	bAction(false),
	bModify(false),
	PlayerActionMontage(nullptr),
	PlayerCommonMontage(nullptr),
	PlayerIdleAnimation(nullptr),
	PlayerMoveAnimation(nullptr),
	PlayerRunAnimation(nullptr),
	ModifyIdleAnimation(nullptr),
	ModifyMoveAnimation(nullptr),
	ModifyRunAnimation(nullptr),
	NonInputAnimState(ENonInputAnimState::None)
{
}

void UPlayerCharacterAnimInstance::SetDefaultProperty(AActor* InOwner)
{
	ACorePlayerCharacter* LocOwnerCharacter = Cast<ACorePlayerCharacter>(InOwner);
	if(!LocOwnerCharacter) return;
	OwnerPlayerCharacter = LocOwnerCharacter;
	UAnimMontage* LocPlayerActionMontage;
	OwnerPlayerCharacter->GetActionMontage(LocPlayerActionMontage);
	SetActionMontage(LocPlayerActionMontage);
	TObjectPtr<UAnimSequence> LocSetSequence = nullptr;
	OwnerPlayerCharacter->GetIdleAnimSequence(LocSetSequence);
	if(LocSetSequence) PlayerIdleAnimation = LocSetSequence;
	OwnerPlayerCharacter->GetRestAnimSequence(LocSetSequence);
	if(LocSetSequence) PlayerRestAnimation = LocSetSequence;	
	OwnerPlayerCharacter->GetMoveAnimSequence(LocSetSequence);
	if(LocSetSequence) PlayerMoveAnimation = LocSetSequence;
	OwnerPlayerCharacter->GetRunAnimSequence(LocSetSequence);
	if(LocSetSequence) PlayerRunAnimation = LocSetSequence;
}

void UPlayerCharacterAnimInstance::SetModifyAnimation(const TObjectPtr<UAnimSequence>& InIdleSequence, const TObjectPtr<UAnimSequence>& InMoveSequence, const TObjectPtr<UAnimSequence>& InRunSequence)
{
	InIdleSequence? ModifyIdleAnimation = InIdleSequence : ModifyIdleAnimation = PlayerIdleAnimation;
	InMoveSequence? ModifyMoveAnimation = InMoveSequence : ModifyMoveAnimation = PlayerMoveAnimation;
	InRunSequence? ModifyRunAnimation = InRunSequence : ModifyRunAnimation = PlayerRunAnimation;
}

void UPlayerCharacterAnimInstance::SetActionMontage(UAnimMontage* InSetActionMontage)
{
	if(InSetActionMontage) PlayerActionMontage = InSetActionMontage;
}

void UPlayerCharacterAnimInstance::SetNonInputAnimState(ENonInputAnimState InSetState)
{
	OwnerPlayerCharacter->SetInputable(false);
	bool LocbChangeState = true;
	switch(InSetState)
	{
		case ENonInputAnimState::Tired:
		{
			LocbChangeState = false; 
			NonInputAnimState = InSetState;
			OwnerPlayerCharacter->SetInputable(true);
			break;
		}
		case ENonInputAnimState::HitBack:
		{
			LocbChangeState = !(NonInputAnimState == ENonInputAnimState::JumpBackStart ||
				NonInputAnimState == ENonInputAnimState::JumpBackLoop ||
				NonInputAnimState == ENonInputAnimState::JumpBackEnd ||
				NonInputAnimState == ENonInputAnimState::GetUp ||
				NonInputAnimState == ENonInputAnimState::Dead);
			break;
		}
		case ENonInputAnimState::JumpBackStart:
		{
			LocbChangeState = !(NonInputAnimState == ENonInputAnimState::JumpBackStart ||
				NonInputAnimState == ENonInputAnimState::JumpBackLoop ||
				NonInputAnimState == ENonInputAnimState::JumpBackEnd ||
				NonInputAnimState == ENonInputAnimState::GetUp ||
				NonInputAnimState == ENonInputAnimState::Dead);
			break;
		}
		case ENonInputAnimState::JumpBackLoop: break;
		case ENonInputAnimState::JumpBackEnd: break;
		case ENonInputAnimState::GetUp: break;
		case ENonInputAnimState::Stun: break;
		case ENonInputAnimState::Dead: break;
		case ENonInputAnimState::Defeat: break;
		case ENonInputAnimState::Victory: break;
		default: NMT_LOG("Non Input Anim state Invalid"); break;
	}
	if(LocbChangeState)
	{
		bPlayerInputable = false;
		NonInputAnimState = InSetState;
	}
}

void UPlayerCharacterAnimInstance::StateChangeTest()
{
}

void UPlayerCharacterAnimInstance::NonInputAnimFinished()
{
	switch(NonInputAnimState)
	{
		case ENonInputAnimState::Tired:
		{
			NonInputAnimState = ENonInputAnimState::None;
			break;
		}
		case ENonInputAnimState::HitBack:
		{
			OwnerPlayerCharacter->SetInputable(true);
			bPlayerInputable = true;
			NonInputAnimState = ENonInputAnimState::None;
			break;	
		}
		case ENonInputAnimState::JumpBackStart: break;
		case ENonInputAnimState::JumpBackLoop: break;
		case ENonInputAnimState::JumpBackEnd:
		{
			SetNonInputAnimState(ENonInputAnimState::GetUp);
			break;
		}
		case ENonInputAnimState::GetUp:
		{
			bPlayerInputable = true;
			NonInputAnimState = ENonInputAnimState::None;
			OwnerPlayerCharacter->SetInputable(true);
			OwnerPlayerCharacter->SetInvincible(false);
			break;
		}
		case ENonInputAnimState::Stun: break;
		case ENonInputAnimState::Dead: break;
		case ENonInputAnimState::Defeat: break;
		case ENonInputAnimState::Victory: break;
		default: NMT_LOG("Non Input Anim state Invalid"); break;
	}
}
