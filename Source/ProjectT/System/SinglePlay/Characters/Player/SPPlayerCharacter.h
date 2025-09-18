// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectT/System/Core/Characters/Player/CorePlayerCharacter.h"
#include "SPPlayerCharacter.generated.h"

UCLASS()
class PROJECTT_API ASPPlayerCharacter : public ACorePlayerCharacter
{
	GENERATED_BODY()
	CONTENT_TYPE_NUMBER(EContentType::PlayerCharacter)
	
public:
	ASPPlayerCharacter();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
};
