// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CoreCharacter.generated.h"

UCLASS()
class PROJECTT_API ACoreCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ACoreCharacter();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void ExecuteClickedEvent(UPrimitiveComponent* InTouchedComponent, FKey InButtonPressed);

	UFUNCTION()
	virtual void ExecuteBeginCursorOverEvent(UPrimitiveComponent* InTouchedComponent);

	UFUNCTION()
	virtual void ExecuteEndCursorOverEvent(UPrimitiveComponent* InTouchedComponent);
	
public:
	virtual void Tick(float DeltaTime) override;

	
};
