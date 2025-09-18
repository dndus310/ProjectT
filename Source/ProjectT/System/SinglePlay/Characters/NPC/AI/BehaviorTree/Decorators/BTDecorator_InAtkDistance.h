// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_InAtkDistance.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTT_API UBTDecorator_InAtkDistance : public UBTDecorator
{
	GENERATED_BODY()

public:
	UBTDecorator_InAtkDistance();
	
protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};
