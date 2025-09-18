// Fill out your copyright notice in the Description page of Project Settings.


#include "SPGameStateBase.h"

#include "ProjectT/System/Core/Common/GlobalUtilsLibrary.h"
#include "ProjectT/System/Core/GameModes/WorldGameMode.h"
#include "ProjectT/System/Core/Interfaces/GlobalObjectProvider.h"
#include "ProjectT/System/Core/Managers/NotificationCenter.h"

ASPGameStateBase::ASPGameStateBase():
	ProcessState(EProcessState::None),
	bOnce(0)
{
	PrimaryActorTick.bCanEverTick = true;
}

void ASPGameStateBase::ChangeProcessState_Implementation(EProcessState NewState)
{
	if(ProcessState == NewState) return;

	ProcessState = NewState;

	if(ChangedProcessEvent.IsBound())
	{
		if(!HasAuthority()) return;

		UWorld* World = UGlobalUtilsLibrary::GetValidWorld(this);
		AWorldGameMode* GM = World->GetAuthGameMode<AWorldGameMode>();
		NMT_CHECKF(GM);

		if(ChangedProcessEvent.IsBound())
		{
			bool bSucceeded = ChangedProcessEvent.Execute(ProcessState);
			IGlobalObjectProvider::Execute_GetNotificationCenter(GetGameInstance())->Post(ENotificationTypes::GameState_Changed);
		}
	}
}

EProcessState ASPGameStateBase::GetProcessState_Implementation()
{
	return ProcessState;
}

void ASPGameStateBase::BeginPlay()
{
	Super::BeginPlay();
}

void ASPGameStateBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if(!bOnce)
	{
		UWorld* World = UGlobalUtilsLibrary::GetValidWorld(this);
		AWorldGameMode* GM = UGlobalUtilsLibrary::GetGameModeChecked<AWorldGameMode>(World);
		ChangedProcessEvent.BindUObject(GM, &AWorldGameMode::ChangedProcessEvent);
		Execute_ChangeProcessState(this, EProcessState::Ready);
		bOnce = true;
		return;
	}
}
