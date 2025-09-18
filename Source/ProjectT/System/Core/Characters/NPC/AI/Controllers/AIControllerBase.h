// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Runtime/AIModule/Classes/AIController.h"
#include "ProjectT/Data/Npc/NpcDataModel.h"
#include "AIControllerBase.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDetectTarget);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnForgetTarget);

enum class EEffectSubTypes : uint8;

UCLASS()
class PROJECTT_API AAIControllerBase : public AAIController
{
	GENERATED_BODY()

public:
	AAIControllerBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void PostInitializeComponents() override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void Tick(float DeltaTime) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	UFUNCTION(BlueprintCallable)
	void UpdateTargetInfo(AActor* InTarget);
	
	UFUNCTION(BlueprintCallable)
	void ChangeNpcStateKey(const ENpcState InState);

	UFUNCTION(BlueprintCallable)
	virtual void StartAI();

	UFUNCTION(BlueprintCallable)
	virtual void StopAI(const FString& Reason, const bool bControlPerception = true);

	class AActor* GetTargetActor() const;
	FORCEINLINE void SetCanDetect(bool InbCanDetect) {  bCanDetect = InbCanDetect; }
	FORCEINLINE bool GetCanDetect() const {  return bCanDetect; }
	virtual void ActivateBehaviorTree(APawn* InPawn);
	void UpdateTargetLocation(const FVector& InLocation);
	void UpdateTargetDirection(const AActor* InTarget);
	void UpdateTargetDirection(const FVector& InTargetDirection);
	void UpdateReturnLocation(const FVector& InLocation);
	void UpdateNextPatrolType(const EPatrolType InPatrolType);
	void UpdateDoubtState(const EDoubtState InDoubtState);
	void SetEnableAttack(const bool InbEnable);
	void SetCanRotateWhileAttack(const bool InbEnable);
	
protected:
	virtual void BeginPlay() override;
	class UBlackboardComponent* GetCheckedBlackboardComponent();

protected:
	UPROPERTY(BlueprintAssignable) 
	FOnDetectTarget OnDetectTarget;
	
	UPROPERTY(BlueprintAssignable)
	FOnForgetTarget OnForgetTarget;

	UPROPERTY()
	TObjectPtr<class UCrowdFollowingComponent> CrowdFollowingComponent;

	uint8 bCanDetect : 1;
	
private:

	UPROPERTY()
	TObjectPtr<class UBehaviorTree> BehaviorTree;
	
};

