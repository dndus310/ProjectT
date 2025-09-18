// Fill out your copyright notice in the Description page of Project Settings.


#include "SPSneako.h"
#include "ProjectT/System/Core/Characters/NPC/CoreNpc.h"
#include "ProjectT/System/Core/Characters/Player/Animations/AnimInstance/PlayerCharacterAnimInstance.h"
#include "ProjectT/System/Core/Characters/Skill/CoreSkill.h"
#include "ProjectT/System/Core/Common/GlobalUtilsLibrary.h"
#include "ProjectT/System/Core/GameModes/WorldGameMode.h"

ASPSneako::ASPSneako() :
	HeadHideMaterialInstance(nullptr),
	HeadMaterialInstance(nullptr),
	SkillIdleAnimation(nullptr),
	SkillMoveAnimation(nullptr)
{
	PrimaryActorTick.bCanEverTick = true;
}

void ASPSneako::BeginPlay()
{
	Super::BeginPlay();
	PlayerAnimInstance->SetModifyAnimation(SkillIdleAnimation, SkillMoveAnimation);
}

void ASPSneako::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}

void ASPSneako::SetSkillState(EPlayerSkillState InSkillState, FSkill InSkillData)
{
	switch(InSkillState)
	{
		case EPlayerSkillState::Start:
		{
			GetMesh()->SetMaterial(0, HeadHideMaterialInstance);
			WeaponStaticComponent->SetHiddenInGame(true);
			CurrentUsingSkill->StartSkill();
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
			FSkill LocSneakoSkillData;
			CurrentUsingSkill->GetSkillData(LocSneakoSkillData);
			SetSkillState(EPlayerSkillState::Finish, LocSneakoSkillData);
			break;
		}
		case EPlayerSkillState::Release:
		{
			CurrentUsingSkill->ReleaseSkill();
			SetSkillState(EPlayerSkillState::Finish, InSkillData);
			break;
		}
		case EPlayerSkillState::Activate: break;
		case EPlayerSkillState::Finish:
		{
			CurrentUsingSkill->FinishSkill();
			AWorldGameMode* LocGameMode = UGlobalUtilsLibrary::GetGameModeChecked<AWorldGameMode>(this);
			TArray<AActor*> LocNpcCharacters;
			LocGameMode->GetNpcCharacters(LocNpcCharacters);
			for(AActor* LocNpc : LocNpcCharacters)
			{
				TArray<EEffectSubTypes> LocNpcEffectTypes;
				Execute_GetCurrentEffectTypes(LocNpc, LocNpcEffectTypes);
				if(!LocNpcEffectTypes.Contains(EEffectSubTypes::Unrecog))
				{
					TArray<FEffect> LocEffects;
					FEffect LocEffect;
					const FString LocUnrecogString = UGlobalUtilsLibrary::GetEnumToString<EEffectSubTypes>(EEffectSubTypes::Unrecog);
					LocEffect.sEffect = LocUnrecogString;
					LocEffects.Add(LocEffect);
					Execute_ApplyRecoveryEvent(LocNpc, LocEffects);
				}
			}
			SetSkillCoolTime(CurrentUsingSkill.GetClass(),InSkillData);
			SetCurrentSkill(false);
			GetMesh()->SetMaterial(0, HeadMaterialInstance);
			WeaponStaticComponent->SetHiddenInGame(false);
			PlayerAnimInstance->SetModifyState(false);
			break;
		}
		default: NMT_LOG("SneakoSkill State None"); break;
	}	
}

void ASPSneako::SetMeleeAtkSkillState(EPlayerSkillState InSkillState, FSkill InSkillData)
{
	Super::SetMeleeAtkSkillState(InSkillState, InSkillData);
}

void ASPSneako::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASPSneako::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

