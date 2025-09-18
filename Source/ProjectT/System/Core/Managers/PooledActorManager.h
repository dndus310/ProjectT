// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "PooledActorManager.generated.h"

UCLASS()
class PROJECTT_API UPooledActorManager : public UObject
{
	GENERATED_BODY()

public:
	UPooledActorManager();
	void ReleaseManager();
	void CreatePool(const TSubclassOf<AActor>& ActorClass, int32 PoolSize, TArray<AActor*>* OutActors = nullptr);
	void Push(AActor* Object);
	AActor* Obtain(const TSubclassOf<AActor>& ActorClass);
	
private:
	AActor* Create(const TSubclassOf<AActor>& ActorClass);
	
private:
	TMap<TSubclassOf<AActor>, TArray<AActor*>> Pool;
};
