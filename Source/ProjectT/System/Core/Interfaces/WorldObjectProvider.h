// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "WorldObjectProvider.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UWorldObjectProvider : public UInterface
{
	GENERATED_BODY()
};

class PROJECTT_API IWorldObjectProvider
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, meta = (Category = "WorldObjectProvider"))
	class USequenceManager* GetSequenceManager();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, meta = (Category = "WorldObjectProvider"))
	class UPooledActorManager* GetPooledActorManager();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, meta = (Category = "WorldObjectProvider"))
	class UQuestManager* GetQuestManager();
};
