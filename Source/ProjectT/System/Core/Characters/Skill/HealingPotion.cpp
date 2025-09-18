// Fill out your copyright notice in the Description page of Project Settings.


#include "HealingPotion.h"

#include "NiagaraFunctionLibrary.h"
#include "Components/AudioComponent.h"
#include "ProjectT/System/Core/Characters/CombatCharacter.h"
#include "ProjectT/System/Core/Common/GlobalUtilsLibrary.h"
#include "ProjectT/System/Core/Interfaces/Damageable.h"
#include "ProjectT/System/Core/Managers/SoundManager.h"

AHealingPotion::AHealingPotion() :
	HealFXAsset(nullptr),
	HealSFXComponent(nullptr)
{
	SkillPoolsize = 3;
	SkillRowName = "313403";
	SkillId = 313403;
	InitializeSkillData();
	PrimaryActorTick.bCanEverTick = true;
}

void AHealingPotion::BeginPlay()
{
	Super::BeginPlay();	
}

void AHealingPotion::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AHealingPotion::ActivateSkill()
{
	if(!SkillOwner) return;
	MakeSkillNoise();
	if(HealSFXComponent) HealSFXComponent.Get()->Stop();
	HealSFXComponent = UGlobalUtilsLibrary::GetSharedSoundManager(this)->PlaySound3DAttached(TEXT("831021"), SkillOwner->GetRootComponent(), EAttachLocation::Type::SnapToTarget,TEXT("None"));
	IDamageable::Execute_ApplyDamageEvent(SkillOwner, this, GetActorLocation(), SkillData.fDamage, SkillEffects);
	USkeletalMeshComponent* LocMesh = Cast<ACombatCharacter>(SkillOwner)->GetMesh();
	UNiagaraFunctionLibrary::SpawnSystemAttached(HealFXAsset, LocMesh, FName("SOK_Root_Fx"),FVector::Zero(),FRotator::ZeroRotator,EAttachLocation::Type::SnapToTarget,true,true);
	FinishSkill();
}

