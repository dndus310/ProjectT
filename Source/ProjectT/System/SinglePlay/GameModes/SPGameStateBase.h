// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectT/System/Core/GameModes/CoreGameState.h"
#include "SPGameStateBase.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTT_API ASPGameStateBase : public ACoreGameState
{
	GENERATED_BODY()

public:
	ASPGameStateBase();
	virtual void ChangeProcessState_Implementation(EProcessState NewState) override;
	virtual EProcessState GetProcessState_Implementation() override;
	
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

protected:
	FOnChangedProcessEvent ChangedProcessEvent;
	EProcessState ProcessState;
	uint8 bOnce:1;
};
