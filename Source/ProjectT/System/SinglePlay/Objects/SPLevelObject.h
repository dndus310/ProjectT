// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectT/System/Core/Objects/LevelObject/CoreInteractionObject.h"
#include "SPLevelObject.generated.h"

UCLASS()
class PROJECTT_API ASPLevelObject : public ACoreInteractionObject
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASPLevelObject();
		
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
