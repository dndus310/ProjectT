// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_RotateToDefault.generated.h"


struct FBTRotateToDefaultMemory
{
	TWeakObjectPtr<class ASPNpcStationary> NpcStationary;
	FRotator CurrentRotation;
};

UCLASS()
class PROJECTT_API UBTTask_RotateToDefault : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_RotateToDefault();
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

protected:
	virtual uint16 GetInstanceMemorySize() const override;
};
