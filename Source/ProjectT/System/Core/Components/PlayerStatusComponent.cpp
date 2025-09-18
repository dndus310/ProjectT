// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerStatusComponent.h"
#include "ProjectT/System/Core/Characters/Player/CorePlayerCharacter.h"
#include "ProjectT/System/Core/Common/GlobalUtilsLibrary.h"
#include "ProjectT/System/Core/Managers/CoreCheatManager.h"

UPlayerStatusComponent::UPlayerStatusComponent()
{

	PrimaryComponentTick.bCanEverTick = true;

}

void UPlayerStatusComponent::BeginPlay()
{
	Super::BeginPlay();

}

void UPlayerStatusComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UPlayerStatusComponent::InitializeStatusData(FPlayerCharacter InPlayerData)
{
	PreviousPlayerStatus = InPlayerData;
	CurrentPlayerStatus = InPlayerData;
}

void UPlayerStatusComponent::ResetStatusDefault()
{
	CurrentPlayerStatus = PreviousPlayerStatus;
}

bool UPlayerStatusComponent::ApplyDamage(float InApplyDamage)
{
	ACorePlayerCharacter* LocOwner = Cast<ACorePlayerCharacter>(GetOwner());
	if(!LocOwner) return false;
	bool LocPlayerInvincible;
	LocOwner->GetInvincible(LocPlayerInvincible);

	UCoreCheatManager* CCM = UGlobalUtilsLibrary::GetSharedCoreCheatManager(this);
	LocPlayerInvincible = CCM->GetDebugStatus().bDebugPlayerInvincibleHP ? true : LocPlayerInvincible;
	
	if(!LocPlayerInvincible)
	{
		CurrentPlayerStatus.fCharHp = CurrentPlayerStatus.fCharHp - InApplyDamage;
		if(UpdatePlayerHpPercent.IsBound()) UpdatePlayerHpPercent.Broadcast(CurrentPlayerStatus.fCharHp / PreviousPlayerStatus.fCharHp);
	}
	return CurrentPlayerStatus.fCharHp <= 0.f;
}

void UPlayerStatusComponent::SpendPlayerStamina(bool InbSpend)
{
	float LocUpdateStaminaValue;
	if(InbSpend)
	{
		LocUpdateStaminaValue = -20.f * GetWorld()->DeltaTimeSeconds;	
	}
	else
	{
		LocUpdateStaminaValue = PreviousPlayerStatus.fStaRecovery * GetWorld()->DeltaTimeSeconds;
	}
	CurrentPlayerStatus.fStamina = CurrentPlayerStatus.fStamina + LocUpdateStaminaValue;
	if(UpdatePlayerStaminaPercent.IsBound()) UpdatePlayerStaminaPercent.Broadcast(CurrentPlayerStatus.fStamina / PreviousPlayerStatus.fStamina);
}

FPlayerCharacter UPlayerStatusComponent::GetPreviousPlayerStatus()
{
	return PreviousPlayerStatus;
}

FPlayerCharacter UPlayerStatusComponent::GetCurrentPlayerStatus()
{
	return CurrentPlayerStatus;
}

float UPlayerStatusComponent::GetStatusPreviousCharHp()
{
	return PreviousPlayerStatus.fCharHp;
}

float UPlayerStatusComponent::GetStatusCharHp()
{
	return CurrentPlayerStatus.fCharHp;
}

void UPlayerStatusComponent::SetStatusCharHp(float InSetValue)
{
	CurrentPlayerStatus.fCharHp = InSetValue;
}

float UPlayerStatusComponent::GetStatusPreviousCharMove()
{
	UCoreCheatManager* CCM = UGlobalUtilsLibrary::GetSharedCoreCheatManager(this);
	return PreviousPlayerStatus.fCharMove * (CCM->GetDebugStatus().bDebugPlayerSpeed ? 2.f : 1.f);
}

float UPlayerStatusComponent::GetStatusCharMove()
{
	UCoreCheatManager* CCM = UGlobalUtilsLibrary::GetSharedCoreCheatManager(this);
	return CurrentPlayerStatus.fCharMove * (CCM->GetDebugStatus().bDebugPlayerSpeed ? 2.f : 1.f);
}

void UPlayerStatusComponent::SetStatusCharMove(float InSetValue)
{
	CurrentPlayerStatus.fCharMove = InSetValue;
}

void UPlayerStatusComponent::AddStatusCharMove(float InAddValue)
{
	CurrentPlayerStatus.fCharMove = GetStatusCharMove() + InAddValue;
}

float UPlayerStatusComponent::GetStatusCharMoveSound()
{
	return CurrentPlayerStatus.fCharMoveSound;
}

void UPlayerStatusComponent::SetStatusCharMoveSound(float InSetValue)
{
	CurrentPlayerStatus.fCharMoveSound = InSetValue;
}

float UPlayerStatusComponent::GetStatusPreviousCharRun()
{
	UCoreCheatManager* CCM = UGlobalUtilsLibrary::GetSharedCoreCheatManager(this);
	return CurrentPlayerStatus.fCharRun * (CCM->GetDebugStatus().bDebugPlayerSpeed ? 2.f : 1.f);
}

float UPlayerStatusComponent::GetStatusCharRun()
{
	UCoreCheatManager* CCM = UGlobalUtilsLibrary::GetSharedCoreCheatManager(this);
	return CurrentPlayerStatus.fCharRun * (CCM->GetDebugStatus().bDebugPlayerSpeed ? 2.f : 1.f);
}

void UPlayerStatusComponent::SetStatusCharRun(float InSetValue)
{
	CurrentPlayerStatus.fCharRun = InSetValue;
}

void UPlayerStatusComponent::AddStatusCharRun(float InAddValue)
{
	CurrentPlayerStatus.fCharRun = GetStatusCharRun() + InAddValue;
}

float UPlayerStatusComponent::GetStatusCharRunSound()
{
	return CurrentPlayerStatus.fCharRunSound;
}

void UPlayerStatusComponent::SetStatusCharRunSound(float InSetValue)
{
	CurrentPlayerStatus.fCharRunSound = InSetValue;
}

float UPlayerStatusComponent::GetStatusPreviousStamina()
{
	return PreviousPlayerStatus.fStamina;
}

float UPlayerStatusComponent::GetStatusStamina()
{
	return CurrentPlayerStatus.fStamina;
}

void UPlayerStatusComponent::SetStatusStamina(float InSetValue)
{
	CurrentPlayerStatus.fStamina = InSetValue;
}

float UPlayerStatusComponent::GetStatusStaRecovery()
{
	return CurrentPlayerStatus.fStaRecovery;
}

void UPlayerStatusComponent::SetStatusStaRecovery(float InSetValue)
{
	CurrentPlayerStatus.fStaRecovery = InSetValue;
}

