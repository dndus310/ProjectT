// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SPObjectProvider.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, Blueprintable)
class USPObjectProvider : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PROJECTT_API ISPObjectProvider
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, meta = (Category = "SPObjectProvider"))
	class USPWidgetManager* GetSPWidgetManager();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, meta = (Category = "SPObjectProvider"))
	class USPEnemyVisionManager* GetEnemyVisionManager();
};
