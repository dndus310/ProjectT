// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ResourceManager.generated.h"


DECLARE_DELEGATE(FOnPreLoadAssetComplete)

UCLASS()
class PROJECTT_API UResourceManager : public UObject
{
	GENERATED_BODY()

public:
	UResourceManager();
	void Initialize(const UObject* WorldObjectContext);
	void ReleaseManager();
	void RequestPreLoad(const FOnPreLoadAssetComplete& Callback);
	
private:
	UPROPERTY()
	TObjectPtr<UObject> OuterObject;
	
	UPROPERTY()
	TMap<FSoftObjectPath, TObjectPtr<UObject>> Resources;

	FOnPreLoadAssetComplete OnPreLoadAssetComplete;
};
