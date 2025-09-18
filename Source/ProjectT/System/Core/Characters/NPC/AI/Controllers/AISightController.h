// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIPerceptionController.h"
#include "AISightController.generated.h"

UCLASS()
class PROJECTT_API AAISightController : public AAIPerceptionController
{
	GENERATED_BODY()

public:
	AAISightController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void BeginPlay() override;

public:
	virtual void OnPossess(APawn* InPawn) override;
	virtual void Tick(float DeltaTime) override;
	
public:
	UPROPERTY(EditAnywhere, Category = "AIPerception")
	class UAISenseConfig_Sight* SightConfig;
};

