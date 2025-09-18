// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "ProjectT/Data/Npc/NpcDataModel.h"
#include "BTTask_RotateHead.generated.h"

struct FBTRotateHeadMemory
{
	TWeakObjectPtr<class ACoreNpc> Npc;
};

UCLASS()
class PROJECTT_API UBTTask_RotateHead : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTask_RotateHead();
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

protected:
	virtual uint16 GetInstanceMemorySize() const override;
	
public:
	UPROPERTY(EditAnywhere)
	EHeadRotationType HeadRotationType;

};
