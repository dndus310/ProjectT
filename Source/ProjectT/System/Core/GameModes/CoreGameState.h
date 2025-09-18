// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "ProjectT/System/Core/Interfaces/ProcessStateControl.h"
#include "CoreGameState.generated.h"


DECLARE_DELEGATE_RetVal_OneParam(bool, FOnChangedProcessEvent, const EProcessState);

UCLASS()
class PROJECTT_API ACoreGameState : public AGameState, public IProcessStateControl
{
	GENERATED_BODY()
	
public:
	ACoreGameState();
	virtual void ChangeProcessState_Implementation(EProcessState NewState) override;
	virtual EProcessState GetProcessState_Implementation() override;
	
protected:
	virtual void BeginPlay() override;
};
