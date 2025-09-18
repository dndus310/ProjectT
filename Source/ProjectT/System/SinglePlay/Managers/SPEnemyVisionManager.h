// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "SPEnemyVisionManager.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTT_API USPEnemyVisionManager : public UObject
{
	GENERATED_BODY()

public:
	USPEnemyVisionManager();

	void InitializeManager(const UObject* WorldObjectContext);
	void ReleaseManager();
	void RegisterFocused(class ACoreNpc* InDetectNpc);
	void RegisterDetection(class ACoreNpc* InDetectNpc);
	void UnregisterDetection(class ACoreNpc* InDetectNpc);
	void UnregisterFocused();
	void UnregisterAllDetection();
	bool IsFocusedNpc(class ACoreNpc* InNpc) const;
	
protected:
	void UpdateSightState();
	
private:
	UPROPERTY()
	TObjectPtr<UObject> OuterObject;
	
	UPROPERTY()
	TObjectPtr<class ACoreNpc> FocusedNpc;

	UPROPERTY()
	TArray<TObjectPtr<class ACoreNpc>> DetectNpcList;
};
