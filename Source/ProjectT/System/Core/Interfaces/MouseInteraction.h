// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MouseInteraction.generated.h"

UINTERFACE()
class UMouseInteraction : public UInterface
{
	GENERATED_BODY()
};

class PROJECTT_API IMouseInteraction
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, meta = (Category = "Damageable"))
	void RefreshCursorHitResult(FHitResult InHitResult);
	
};
