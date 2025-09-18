// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "LoadableObject.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class ULoadableObject : public UInterface
{
	GENERATED_BODY()
};

class PROJECTT_API ILoadableObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "LoadableObject")
	bool HasFinishedLoading();

	virtual bool HasFinishedLoading() const
	{
		return false;
	}
};
