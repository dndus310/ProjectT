// Fill out your copyright notice in the Description page of Project Settings.


#include "SPPyroMania.h"
#include "ProjectT/System/Core/Characters/Player/Animations/AnimInstance/PlayerCharacterAnimInstance.h"
#include "ProjectT/System/Core/Characters/Skill/PyroAtk.h"
#include "ProjectT/System/Core/Components/PlayerStatusComponent.h"

ASPPyroMania::ASPPyroMania()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ASPPyroMania::BeginPlay()
{
	Super::BeginPlay();
}

void ASPPyroMania::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}

void ASPPyroMania::SetSkillState(EPlayerSkillState InSkillState, FSkill InSkillData)
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
			StartMouseCursorLinear(true);
			CurrentUsingSkill->AimSkill();
			if(bMovingAction)
			{
				PlayerAnimInstance->SetActionState(false);
			}
			else
			{
				SetInputable(false);
				GetWorld()->GetTimerManager().ClearTimer(MeshRotateCalibrateTimerHandle);
				GetWorld()->GetTimerManager().SetTimer(MouseCursorTimerHandle, this, &ASPPyroMania::RotateMeshToMouseCursor, GetWorld()->DeltaTimeSeconds, true);
			}
			break;
		}
		case EPlayerSkillState::Cancel:
		{
			StartMouseCursorLinear(false);
			GetWorld()->GetTimerManager().ClearTimer(MouseCursorTimerHandle);
			break;
		}
		case EPlayerSkillState::Release:
		{
			StartMouseCursorLinear(false);
			GetWorld()->GetTimerManager().ClearTimer(MouseCursorTimerHandle);
			break;
		}
		case EPlayerSkillState::Activate:
		{
			CurrentUsingSkill->ActivateSkill();
			StartMouseCursorLinear(false);
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
					PlayerStatusComponent->SetStatusCharMove(PlayerStatusComponent->GetStatusPreviousCharMove());
					PlayerAnimInstance->SetActionState(false);
					SetInputable(true);
				}
			}));
			break;
		}
		case EPlayerSkillState::Finish: break;
		default: NMT_LOG("PyroSkill State None"); break;
	}
}

void ASPPyroMania::SetMeleeAtkSkillState(EPlayerSkillState InSkillState, FSkill InSkillData)
{
	Super::SetMeleeAtkSkillState(InSkillState, InSkillData);
}

void ASPPyroMania::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASPPyroMania::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}