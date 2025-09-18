// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_RotateToTargetDirection.generated.h"

/**
 * 
 */

struct FBTRotateToTargetDirMemory
{
	TWeakObjectPtr<class ASPNpcAgent> AgentNpc;
	FRotator TargetRotation;
	uint8 bIsAtTargetRotation : 1;
};

UCLASS()
class PROJECTT_API UBTTask_RotateToTargetDirection : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_RotateToTargetDirection();
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

protected:
	virtual uint16 GetInstanceMemorySize() const override;
	
private:
	void UpdateTargetRotation(UBehaviorTreeComponent& InOwnerComp, uint8* InNodeMemory) const;

};
