// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectT/System/Core/GameModes/CorePlayerController.h"
#include "LobbyPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTT_API ALobbyPlayerController : public ACorePlayerController
{
	GENERATED_BODY()

public:
	ALobbyPlayerController();

	virtual void OnPossess(APawn* InPawn) override;
	
	
};
