// Fill out your copyright notice in the Description page of Project Settings.


#include "CoreInteractionObject.h"

#include "ProjectT/System/Core/Characters/Player/CorePlayerCharacter.h"

ACoreInteractionObject::ACoreInteractionObject() :
	CurrentOwner(nullptr),
	SuccessProgressTime(0.f),
	CurrentProgressRatio(0.f),
	CurrentProgressTime(0.f)	
{
	PrimaryActorTick.bCanEverTick = true;
}

void ACoreInteractionObject::BeginPlay()
{
	Super::BeginPlay();
}

void ACoreInteractionObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACoreInteractionObject::TryInteraction(const FInteractionData InSendData, FOnInteractionSuccess Callback)
{
}

void ACoreInteractionObject::ShowVisual_Implementation()
{
	SetVisibility(true);
}

void ACoreInteractionObject::HideVisual_Implementation()
{
	ResetToDefault();
}

void ACoreInteractionObject::SetVisibility(bool InbVisible)
{
}

void ACoreInteractionObject::ResetToDefault()
{
	SetVisibility(false);
	StartProgressTime(false);
	ProgressTimerHandle.Invalidate();
	CurrentProgressTime = 0.f;
	CurrentProgressRatio = 0.f;
	CurrentOwner = nullptr;
}

void ACoreInteractionObject::StartProgressTime(bool InbStart)
{
	if (InbStart)
	{
		if (!GetWorld()->GetTimerManager().IsTimerActive(ProgressTimerHandle))
			GetWorld()->GetTimerManager().SetTimer(ProgressTimerHandle, this, &ACoreInteractionObject::SetProgressPercent, 1 / 60.f, true);
	}
	else
	{
		GetWorld()->GetTimerManager().ClearTimer(ProgressTimerHandle);
		ProgressTimerHandle.Invalidate();
	}
}

EInteractionObjectType ACoreInteractionObject::GetInteractionObjectType_Implementation()
{
	return InteractionObjectType;
}

void ACoreInteractionObject::SetProgressPercent()
{
	CurrentProgressTime = CurrentProgressTime + 1.f / 60.f;
	CurrentProgressRatio = CurrentProgressTime / SuccessProgressTime;
	if (CurrentProgressRatio > 1.f)
	{
		GetWorld()->GetTimerManager().ClearTimer(ProgressTimerHandle);
		ProgressTimerHandle.Invalidate();
		//CurrentOwner->AcceptObjectInteraction_Implementation(true, true);
		ResetToDefault();
	}
}
