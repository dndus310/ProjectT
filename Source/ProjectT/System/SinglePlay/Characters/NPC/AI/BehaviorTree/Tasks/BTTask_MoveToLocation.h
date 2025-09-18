// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_MoveTo.h"
#include "Navigation/PathFollowingComponent.h"

#include "BTTask_MoveToLocation.generated.h"


UCLASS()
class PROJECTT_API UBTTask_MoveToLocation : public UBTTask_MoveTo
{
	GENERATED_BODY()

public:
	UBTTask_MoveToLocation();
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;

private:
	void ArriveToTargetLocation(UBehaviorTreeComponent& OwnerComp) const;

private:
	TObjectPtr<class ASPNpcAgent> AgentNpc;
};
