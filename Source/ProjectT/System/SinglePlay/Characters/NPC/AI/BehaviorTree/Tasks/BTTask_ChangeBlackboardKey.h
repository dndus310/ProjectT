// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_ChangeBlackboardKey.generated.h"


enum class ENpcState : uint8;
enum class EPatrolType : uint8;

UCLASS()
class PROJECTT_API UBTTask_ChangeBlackboardKey : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_ChangeBlackboardKey();

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

public:
	UPROPERTY(EditAnywhere, Category = "BlackBoard")
	FBlackboardKeySelector KeySelector;

	UPROPERTY(EditAnywhere, Category = "Value", meta = (EditCondition = "bIsBool", EditConditionHides))
	uint8 BoolValue : 1;

	UPROPERTY(EditAnywhere, Category = "Value", meta = (EditCondition = "bIsVector", EditConditionHides))
	FVector VectorValue;

	UPROPERTY(EditAnywhere, Category = "Value", meta = (EditCondition = "bIsNpcStateEnum", EditConditionHides))
	ENpcState NpcStateEnumValue;

private:
	UPROPERTY()
	uint8 bIsBool : 1;

	UPROPERTY()
	uint8 bIsVector : 1;

	UPROPERTY()
	uint8 bIsNpcStateEnum : 1;

};	
	