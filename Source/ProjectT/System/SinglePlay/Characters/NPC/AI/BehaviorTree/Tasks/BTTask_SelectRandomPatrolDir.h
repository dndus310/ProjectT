// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_SelectRandomPatrolDir.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTT_API UBTTask_SelectRandomPatrolDir : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_SelectRandomPatrolDir();
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

};
