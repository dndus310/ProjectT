// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectT/System/Core/GameModes/CoreGameState.h"
#include "LobbyGameState.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTT_API ALobbyGameState : public ACoreGameState
{
	GENERATED_BODY()

public:
	ALobbyGameState();
	

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

protected:
	
	
};
