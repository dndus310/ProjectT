// Fill out your copyright notice in the Description page of Project Settings.


#include "SPIcien.h"

#include "ProjectT/System/Core/Characters/Player/Animations/AnimInstance/PlayerCharacterAnimInstance.h"
#include "ProjectT/System/Core/Characters/Skill/CoreSkill.h"
#include "ProjectT/System/Core/Components/PlayerStatusComponent.h"
// Temp Only KeyBoard State
#define ONLY_KEYBOARD 1

ASPIcien::ASPIcien() :
	SkillMoveAnimation(nullptr)
{
	PrimaryActorTick.bCanEverTick = true;
}

void ASPIcien::BeginPlay()
{
	Super::BeginPlay();
	PlayerAnimInstance->SetModifyAnimation(nullptr, SkillMoveAnimation);
}

void ASPIcien::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}

void ASPIcien::SetSkillState(EPlayerSkillState InSkillState, FSkill InSkillData)
{
	switch(InSkillState)
	{
		case EPlayerSkillState::Start:
		{
			CurrentUsingSkill->StartSkill();
			SetInputable(false);
			PlayActionMontage(*InSkillData.sSkillName, FOnMontageEnded::CreateLambda([InSkillData, this](UAnimMontage* Montage, bool Interrupted)
			{
				if(!Interrupted)
				{
					SetSkillState(EPlayerSkillState::Activate, InSkillData);
				}
			}));
			PlayerAnimInstance->SetModifyState(true);
			CurrentUsingSkill->SkillTimeEnd.BindLambda([InSkillData , this]()
			{
				SetSkillState(EPlayerSkillState::Finish, InSkillData);
			});						
			break;
		}
		case EPlayerSkillState::Aim: break;
		case EPlayerSkillState::Cancel:
		{
			CurrentUsingSkill->CancelSkill();
			FSkill LocIcienSkillData;
			CurrentUsingSkill->GetSkillData(LocIcienSkillData);
			if(!CurrentUsingSkill->GetActivated()) SetSkillState(EPlayerSkillState::Finish, LocIcienSkillData);
			break;
		}
		case EPlayerSkillState::Release:
		{
			//If Want Release decode underline
			//CurrentUsingSkill->ReleaseSkill();
			//SetSkillState(EPlayerSkillState::Finish, InSkillData);
			break;
		}
		case EPlayerSkillState::Activate:
		{
			PlayerAnimInstance->SetActionState(false);
			CurrentUsingSkill->ActivateSkill();
			SetInputable(true);
			break;
		}
		case EPlayerSkillState::Finish:
		{
			CurrentUsingSkill->FinishSkill();
			SetSkillCoolTime(CurrentUsingSkill.GetClass(),InSkillData);
			SetCurrentSkill(false);
			PlayerAnimInstance->SetModifyState(false);
			break;
		}
		default: NMT_LOG("Icien State None"); break;
	}
}

void ASPIcien::SetMeleeAtkSkillState(EPlayerSkillState InSkillState, FSkill InSkillData)
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
			StartMouseCursorParabola(true, false, false);
			CurrentUsingSkill->StartSkill();
			PlayActionMontage(*InSkillData.sSkillName, FOnMontageEnded::CreateLambda([InSkillData, this](UAnimMontage* Montage, bool Interrupted)
			{
				if(!Interrupted) SetMeleeAtkSkillState(EPlayerSkillState::Finish, InSkillData);
			}));
			break;
		}
		case EPlayerSkillState::Aim: break;
		case EPlayerSkillState::Cancel:
		{
			CurrentUsingSkill->CancelSkill();
			FSkill LocIcienSkillData;
			CurrentUsingSkill->GetSkillData(LocIcienSkillData);
			SetMeleeAtkSkillState(EPlayerSkillState::Finish, LocIcienSkillData);
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
		default: NMT_LOG("IcienAtk State None"); break;
	}
}
void ASPIcien::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASPIcien::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

