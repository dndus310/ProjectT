// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIControllerBase.h"
#include "AIPerceptionController.generated.h"

UCLASS()
class PROJECTT_API AAIPerceptionController : public AAIControllerBase
{
	GENERATED_BODY()

public:
	AAIPerceptionController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void OnPossess(APawn* InPawn) override;
	virtual void Tick(float DeltaTime) override;
	virtual void StartAI() override;
	virtual void StopAI(const FString& InReason, const bool InbControlPerception = true) override;
	virtual void ActivateBehaviorTree(APawn* InPawn) override;
	
	void UpdatePerceptionSystem() const;
	void ForceUpdatePerception() const;
	void SetPerceptionActive(const bool InbEnable);
	FORCEINLINE void SetUnDetect(const bool InbUnDetect) { bUnDetect = InbUnDetect; } 
	
protected:
	virtual void BeginPlay() override;
	void SetupCrowdSettings(const FCrowdSettingsInfo& InCrowdSettings) const;
	
	UFUNCTION()
	void UpdatedTargetPerception(AActor* InTarget, FAIStimulus InStimulus);

	class UAISenseConfig_Sight* AddSightConfig();
	class UAISenseConfig_Hearing* AddHearingConfig();

	uint8 bUnDetect;
};
