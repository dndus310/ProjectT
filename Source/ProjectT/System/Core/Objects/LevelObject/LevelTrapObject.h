// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LevelInteractionObject.h"
#include "LevelTrapObject.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTT_API ALevelTrapObject : public ALevelInteractionObject
{
	GENERATED_BODY()

public:
	virtual void SetVisibility(bool InbVisible) override;
	
};
