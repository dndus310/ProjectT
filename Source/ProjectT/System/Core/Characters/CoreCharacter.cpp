// Fill out your copyright notice in the Description page of Project Settings.


#include "CoreCharacter.h"
#include "ProjectT/ProjectT.h"

ACoreCharacter::ACoreCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ACoreCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void ACoreCharacter::ExecuteClickedEvent(UPrimitiveComponent* InTouchedComponent, FKey InButtonPressed)
{
	NOT_IMPLEMENTED();
}

void ACoreCharacter::ExecuteBeginCursorOverEvent(UPrimitiveComponent* InTouchedComponent)
{
	NOT_IMPLEMENTED();
}

void ACoreCharacter::ExecuteEndCursorOverEvent(UPrimitiveComponent* InTouchedComponent)
{
	NOT_IMPLEMENTED();
}

void ACoreCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

