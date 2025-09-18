// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "ProjectT/Data/Npc/NpcDataModel.h"
#include "BTTask_ChangeAgentAnim.generated.h"


UCLASS()
class PROJECTT_API UBTTask_ChangeAgentAnim : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_ChangeAgentAnim();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

public:
	UPROPERTY(EditAnywhere)
	uint8 bNormal : 1;

	UPROPERTY(EditAnywhere, meta = (EditCondition="bNormal", EditConditionHides = true))
	ENpcNormalAnimState NpcNormalAnimState;
	
	UPROPERTY(EditAnywhere, meta = (EditCondition="!bNormal", EditConditionHides = true))
	ENpcAbnormalAnimState NpcAbnormalAnimState;
};
