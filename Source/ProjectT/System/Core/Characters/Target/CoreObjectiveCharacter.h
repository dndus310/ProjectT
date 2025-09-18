// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectT/System/Core/Characters/CombatCharacter.h"
#include "ProjectT/System/Core/Interfaces/Visualizable.h"
#include "CoreObjectiveCharacter.generated.h"

UCLASS()
class PROJECTT_API ACoreObjectiveCharacter : public ACoreCharacter, public IObjectInteractionControl, public IVisualizable
{
	GENERATED_BODY()

public:
	ACoreObjectiveCharacter();

protected:
	virtual void BeginPlay() override;
	virtual void TryInteraction(const FInteractionData InSendData, FOnInteractionSuccess Callback) override;
	virtual EInteractionObjectType GetInteractionObjectType_Implementation() override;
	virtual void ShowVisual_Implementation() override;
	virtual void HideVisual_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void StartAction();
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void EndAction();

public:
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
protected:
	EInteractionObjectType InteractionObjectType;

private:
	FOnInteractionSuccess OnInteractionSuccess;
	uint8 bActionOnce:1;
};
