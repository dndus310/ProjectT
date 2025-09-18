// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Decorators/BTDecorator_ConditionalLoop.h"
#include "BTDecorator_CheckAtArriveLocationLoop.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTT_API UBTDecorator_CheckAtArriveLocationLoop : public UBTDecorator_ConditionalLoop
{
	GENERATED_BODY()

public:
	UBTDecorator_CheckAtArriveLocationLoop();

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

public:
	UPROPERTY(EditAnywhere)
	float AcceptRadius;

	UPROPERTY(EditAnywhere)
	uint8 bIncludeCapsuleRadius : 1;
};
