// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProjectT/System/Core/Interfaces/Visualizable.h"
#include "CoreObject.generated.h"

UCLASS()
class PROJECTT_API ACoreObject : public AActor, public IVisualizable
{
	GENERATED_BODY()

public:
	ACoreObject();
	virtual void ShowVisual_Implementation() override;
	virtual void HideVisual_Implementation() override;
	virtual void Tick(float DeltaTime) override;
	
	FName GetObjectRowName() const { return ObjectRowName; }

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> MeshSceneComponent;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	FName ObjectRowName;
};
