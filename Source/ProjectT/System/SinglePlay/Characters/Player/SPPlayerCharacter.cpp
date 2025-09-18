// Fill out your copyright notice in the Description page of Project Settings.


#include "SPPlayerCharacter.h"

ASPPlayerCharacter::ASPPlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ASPPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void ASPPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASPPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}