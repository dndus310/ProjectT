// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "ProjectT/Data/Npc/NpcDataModel.h"
#include "BTTask_UpdateDoubtGauge.generated.h"

/**
 * 
 */

struct FBTUpdateDoubtGaugeMemory
{
	EDoubtState DoubtState;
	float MaxDoubtTime;
	float CurrentDoubtTime;
};

UCLASS()
class PROJECTT_API UBTTask_UpdateDoubtGauge : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_UpdateDoubtGauge();
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

protected:
	virtual uint16 GetInstanceMemorySize() const override;
	
};
