// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIPerceptionController.h"
#include "AISightHearingController.generated.h"

UCLASS()
class PROJECTT_API AAISightHearingController : public AAIPerceptionController
{
	GENERATED_BODY()

public:
	AAISightHearingController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void BeginPlay() override;

public:
	virtual void OnPossess(APawn* InPawn) override;
	virtual void Tick(float DeltaTime) override;
	
public:
	UPROPERTY(EditAnywhere, Category = "AIPerception")
	class UAISenseConfig_Sight* SightConfig;

	UPROPERTY(EditAnywhere, Category = "AIPerception")
	class UAISenseConfig_Hearing* HearingConfig;
};
