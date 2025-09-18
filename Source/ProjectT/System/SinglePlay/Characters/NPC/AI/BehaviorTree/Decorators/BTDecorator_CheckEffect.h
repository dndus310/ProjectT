// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_CheckEffect.generated.h"

enum class EEffectSubTypes : uint8;
/**
 * 
 */
UCLASS()
class PROJECTT_API UBTDecorator_CheckEffect : public UBTDecorator
{
	GENERATED_BODY()

public:
	UBTDecorator_CheckEffect();

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

	UPROPERTY(EditAnywhere, Category = "BlackBoard")
	EEffectSubTypes EffectSubType;
};
