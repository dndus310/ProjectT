// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ProcessStateControl.generated.h"

UENUM(Blueprintable)
enum class EProcessState : uint8
{
	None = 0 UMETA(Displayname = "None"),
	Ready UMETA(Displayname = "Ready"),
	Intro UMETA(Displayname = "Intro"),
	PreGame UMETA(Displayname = "PreGame"),
	GamePlaying UMETA(Displayname = "GamePlaying"),
	Victory UMETA(Displayname = "Victory"),
	Defeat UMETA(Displayname = "Defeat"),
	Reward UMETA(Displyname = "Reward"),
	Finish UMETA(Displyname = "Finish"),
};

// This class does not need to be modified.
UINTERFACE(MinimalAPI, Blueprintable)
class UProcessStateControl : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PROJECTT_API IProcessStateControl
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, meta = (Category = "ProcessState"))
	void ChangeProcessState(EProcessState NewState);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, meta = (Category = "ProcessState"))
	EProcessState GetProcessState();

};
