// Fill out your copyright notice in the Description page of Project Settings.


#include "SPNox.h"

#include "ProjectT/System/Core/Characters/Player/Animations/AnimInstance/PlayerCharacterAnimInstance.h"
#include "ProjectT/System/Core/Characters/Skill/CoreSkill.h"
#include "ProjectT/System/Core/Components/PlayerStatusComponent.h"

// Temp Only KeyBoard State
#define ONLY_KEYBOARD 1

ASPNox::ASPNox()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ASPNox::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASPNox::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}

void ASPNox::SetSkillState(EPlayerSkillState InSkillState, FSkill InSkillData)
{
	Super::SetSkillState(InSkillState, InSkillData);
	switch(InSkillState)
	{
	case EPlayerSkillState::Start:
		{
			if(!bMovingAction) PlayActionMontage(*InSkillData.sSkillName,nullptr);
			SetSkillState(EPlayerSkillState::Aim);
			break;
		}
	case EPlayerSkillState::Aim:
		{
			StartMouseCursorParabola(true, true, false);
			CurrentUsingSkill->AimSkill();
			if(bMovingAction)
			{
				PlayerAnimInstance->SetActionState(false);
			}
			else
			{
				SetInputable(false);
				GetWorld()->GetTimerManager().ClearTimer(MeshRotateCalibrateTimerHandle);
				GetWorld()->GetTimerManager().SetTimer(MouseCursorTimerHandle, this, &ASPNox::RotateMeshToMouseCursor, GetWorld()->DeltaTimeSeconds, true);
			}
			break;
		}
	case EPlayerSkillState::Cancel:
		{
			StartMouseCursorParabola(false);
			GetWorld()->GetTimerManager().ClearTimer(MouseCursorTimerHandle);
			break;
		}
	case EPlayerSkillState::Release:
		{
			StartMouseCursorParabola(false);
			GetWorld()->GetTimerManager().ClearTimer(MouseCursorTimerHandle);
			break;
		}
	case EPlayerSkillState::Activate:
		{
			StartMouseCursorParabola(false);
			CurrentUsingSkill->ActivateSkill();
			PlayerAnimInstance->SetActionState(true);
			SetSkillCoolTime(CurrentUsingSkill.GetClass(), InSkillData);
			GetWorld()->GetTimerManager().ClearTimer(MouseCursorTimerHandle);
			SetActorRotationToMouseCursor();
			CalibrateMeshMouseCursor();
			SetInputable(false);
			PlayActionMontage(*InSkillData.sSkillName.Append("Active"),FOnMontageEnded::CreateLambda([this](UAnimMontage* Montage, bool Interrupted)
			{
				if(!Interrupted)
				{
					PlayerStatusComponent->CurrentPlayerStatus.fCharMove = PlayerStatusComponent->PreviousPlayerStatus.fCharMove;
					PlayerAnimInstance->SetActionState(false);
					SetInputable(true);
				}
			}));
			break;
		}
	case EPlayerSkillState::Finish: break;
	default: NMT_LOG("NoxSkill State None"); break;
	}
}

void ASPNox::SetMeleeAtkSkillState(EPlayerSkillState InSkillState, FSkill InSkillData)
{
	switch(InSkillState)
	{
		case EPlayerSkillState::Start:
		{
			if(bCalibrateSkill)
			{
				AActor* LocNearActor = nullptr;
				if(CheckSkillCalibrateRange(InSkillData,LocNearActor))
				{
					CalibrateMeshNearActor(LocNearActor);
				}
				else
				{
					#if ONLY_KEYBOARD
					CalibrateMeshMouseCursor();
					#endif
				}
			}
			else
			{
				#if ONLY_KEYBOARD
				CalibrateMeshMouseCursor();
				#endif	
			}
				
			if(bMovingAction)
			{
				PlayerStatusComponent->CurrentPlayerStatus.fCharMove = PlayerStatusComponent->CurrentPlayerStatus.fCharMove * MeleeAtkDecreaseSpeedRatio; 	
			}
			else
			{
				SetInputable(false);	
			}
			CurrentUsingSkill->StartSkill();
			PlayActionMontage(*InSkillData.sSkillName,FOnMontageEnded::CreateLambda([InSkillData, this](UAnimMontage* Montage, bool Interrupted)
			{
				if(!Interrupted) SetMeleeAtkSkillState(EPlayerSkillState::Finish,InSkillData);
			}));
			break;
		}
		case EPlayerSkillState::Aim: break;
		case EPlayerSkillState::Cancel:
		{
			SetSkillCoolTime(CurrentUsingSkill.GetClass(), InSkillData);
			CurrentUsingSkill->CancelSkill();
			SetCurrentSkill(false);
			PlayerAnimInstance->SetActionState(false);
			if(bMovingAction)
			{
				SetActorRotation(GetMesh()->GetComponentRotation());
				PlayerStatusComponent->CurrentPlayerStatus.fCharMove = PlayerStatusComponent->PreviousPlayerStatus.fCharMove; 	
			}
			else
			{
				SetInputable(true);	
			}
			break;
		}
		case EPlayerSkillState::Release: break;
		case EPlayerSkillState::Finish:
		{
			SetSkillCoolTime(CurrentUsingSkill.GetClass(), InSkillData);
			SetCurrentSkill(false);
			PlayerAnimInstance->SetActionState(false);
			if(bMovingAction)
			{
				SetActorRotation(GetMesh()->GetComponentRotation());
				PlayerStatusComponent->CurrentPlayerStatus.fCharMove = PlayerStatusComponent->PreviousPlayerStatus.fCharMove; 	
			}
			else
			{
				SetInputable(true);	
			}
			break;
		}
	default: NMT_LOG("NoxAtk State None"); break;
	}
}

void ASPNox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASPNox::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

