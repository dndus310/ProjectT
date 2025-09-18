// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SPNpcAgent.h"
#include "SPNpcPatrol.generated.h"

UCLASS()
class PROJECTT_API ASPNpcPatrol : public ASPNpcAgent
{
	GENERATED_BODY()

public:
	ASPNpcPatrol();
	virtual void PostInitializeComponents() override;
	virtual void Tick(float DeltaTime) override;
	virtual void ApplyDamageEvent_Implementation(AActor* InOccurActor, const FVector& InOriginLocation, const float InDamage, const TArray<FEffect>& InSkillEffects) override;
	virtual void ApplyRecoveryEvent_Implementation(const TArray<FEffect>& InReleaseEffects) override;
	virtual void ActivateCharacter(const bool InbEnable) override;
	virtual void ForgetTarget_Implementation(AActor* InTarget) override;
	virtual void ChangeNpcState(const ENpcState InState) override;
	
	UFUNCTION()
	EPatrolType SelectNextPatrolType();

	UFUNCTION()
	void ChangePatrolType(bool InbForcedAction = false, EPatrolType InActionType = EPatrolType::None);
	
protected:
	virtual void BeginPlay() override;
	virtual void Respawn() override;
	
private:
	void InitializePatrolInfo();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component")
    TObjectPtr<class UPathFollowComponent> PathFollowComponent;

private:
	UPROPERTY()
	EPatrolType CurrentPatrolType;
	
	TArray<TPair<EPatrolType, int32>> PatrolTypeWeights;
};
