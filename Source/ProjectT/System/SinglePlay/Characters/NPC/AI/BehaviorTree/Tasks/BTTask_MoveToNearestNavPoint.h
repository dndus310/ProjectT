// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_MoveToNearestNavPoint.generated.h"


UCLASS()
class PROJECTT_API UBTTask_MoveToNearestNavPoint : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_MoveToNearestNavPoint();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UFUNCTION()
	void OnMoveFinished();
	
public:
	UPROPERTY(EditAnywhere)
	float NavSearchExtent2D;
	
protected:
	UPROPERTY()
	TObjectPtr<class UBehaviorTreeComponent> CachedOwnerBTComp;
	
	FLatentActionInfo LatentInfo;
};
