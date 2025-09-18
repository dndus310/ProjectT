// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "SkillObjectAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTT_API USkillObjectAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	USkillObjectAnimInstance();
	void SetDefaultProperty(TObjectPtr<UAnimSequence> InIdleSequence, TObjectPtr<UAnimMontage> InObjectMontage);
	TObjectPtr<UAnimMontage> GetSkillObjectMontage() { return ObjectMontage; }

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PlayerAnimationState)
	TObjectPtr<UAnimMontage> ObjectMontage;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PlayerAnimationState)
	TObjectPtr<UAnimSequence> ObjectIdleAnimation;
};
