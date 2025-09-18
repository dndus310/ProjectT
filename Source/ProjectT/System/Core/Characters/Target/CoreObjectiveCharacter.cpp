// Fill out your copyright notice in the Description page of Project Settings.


#include "CoreObjectiveCharacter.h"
#include "ProjectT/ProjectT.h"

ACoreObjectiveCharacter::ACoreObjectiveCharacter():
	bActionOnce(0)
{
	PrimaryActorTick.bCanEverTick = true;
	InteractionObjectType = EInteractionObjectType::ObjectiveCharacter;
}

void ACoreObjectiveCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void ACoreObjectiveCharacter::TryInteraction(const FInteractionData InSendData, FOnInteractionSuccess Callback)
{
	if(bActionOnce)
	{
		if(Callback.IsBound()) Callback.Execute(EInteractionResultType::SuccessComplete);
		return;
	}
	if(OnInteractionSuccess.IsBound()) return;
	bActionOnce = true;
	OnInteractionSuccess = Callback;
	Callback.Execute(EInteractionResultType::SuccessPre);
	StartAction();
}

EInteractionObjectType ACoreObjectiveCharacter::GetInteractionObjectType_Implementation()
{
	return InteractionObjectType;
}

void ACoreObjectiveCharacter::ShowVisual_Implementation()
{	
}

void ACoreObjectiveCharacter::HideVisual_Implementation()
{	
}

void ACoreObjectiveCharacter::StartAction_Implementation()
{
}

void ACoreObjectiveCharacter::EndAction_Implementation()
{
	if(!OnInteractionSuccess.IsBound()) return;
	OnInteractionSuccess.Execute(EInteractionResultType::SuccessComplete);
}

void ACoreObjectiveCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACoreObjectiveCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

