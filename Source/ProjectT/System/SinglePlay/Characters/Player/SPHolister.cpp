// Fill out your copyright notice in the Description page of Project Settings.


#include "SPHolister.h"

ASPHolister::ASPHolister()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ASPHolister::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASPHolister::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}

void ASPHolister::SetSkillState(EPlayerSkillState InSkillState, FSkill InSkillData)
{
	//Super::SetSkillState(InSkillState, InSkillData);
	Super::SetMeleeAtkSkillState(InSkillState, InSkillData);
}

void ASPHolister::SetMeleeAtkSkillState(EPlayerSkillState InSkillState, FSkill InSkillData)
{
	Super::SetMeleeAtkSkillState(InSkillState, InSkillData);
}

void ASPHolister::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASPHolister::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

