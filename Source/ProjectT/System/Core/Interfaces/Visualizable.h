// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Visualizable.generated.h"

UINTERFACE()
class UVisualizable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PROJECTT_API IVisualizable
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Visualizable")
	void ShowVisual();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Visualizable")
	void HideVisual();
};
