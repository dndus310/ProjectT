// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_CheckAtArriveLocation.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTT_API UBTDecorator_CheckAtArriveLocation : public UBTDecorator
{
	GENERATED_BODY()

public:
	UBTDecorator_CheckAtArriveLocation();

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

public:
	UPROPERTY(EditAnywhere, Category = Blackboard)
	FBlackboardKeySelector BlackboardKey;
	
	UPROPERTY(EditAnywhere)
	float AcceptRadius;

	UPROPERTY(EditAnywhere)
	uint8 bIncludeCapsuleRadius : 1;
	
};
