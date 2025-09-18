// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NPCSpawner.generated.h"

UCLASS()
class PROJECTT_API ANPCSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	ANPCSpawner();	
	virtual void Destroyed() override;
	
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	
private:
	void SpawnNPCByTable();
#endif
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<USceneComponent> RootSceneComponent;
	
private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<AActor> SpawnedNPCActor;
};