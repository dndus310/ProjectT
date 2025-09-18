// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CoreObjectiveCharacter.h"
#include "CoreSaveCharacter.generated.h"

struct FInteractionData;
enum class EObjectiveSubTypes : uint8;

UCLASS()
class PROJECTT_API ACoreSaveCharacter : public ACoreObjectiveCharacter
{
	GENERATED_BODY()

public:
	ACoreSaveCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void EndAction_Implementation() override;

protected:
	virtual void BeginPlay() override;

private:
	EObjectiveSubTypes ObjectiveType;
	
};
