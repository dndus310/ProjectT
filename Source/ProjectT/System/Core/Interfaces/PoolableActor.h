// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PoolableActor.generated.h"

UINTERFACE()
class UPoolableActor : public UInterface
{
	GENERATED_BODY()
};

class PROJECTT_API IPoolableActor
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, meta = (Category = "PoolableActor"))
	void Enable();
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, meta = (Category = "PoolableActor"))
	void Disable();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, meta = (Category = "PoolableActor"))
	void Pushed();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, meta = (Category = "PoolableActor"))
	void Obtained();
};
