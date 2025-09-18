// Fill out your copyright notice in the Description page of Project Settings.


#include "CoreGameState.h"

ACoreGameState::ACoreGameState()
{
	
}

void ACoreGameState::ChangeProcessState_Implementation(EProcessState NewState)
{
	IProcessStateControl::ChangeProcessState_Implementation(NewState);
}

EProcessState ACoreGameState::GetProcessState_Implementation()
{
	return IProcessStateControl::GetProcessState_Implementation();
}

void ACoreGameState::BeginPlay()
{
	Super::BeginPlay();
}

