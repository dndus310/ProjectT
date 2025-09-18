// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillObjectAnimInstance.h"

USkillObjectAnimInstance::USkillObjectAnimInstance() :
	ObjectMontage(nullptr),
	ObjectIdleAnimation(nullptr)
{
}

void USkillObjectAnimInstance::SetDefaultProperty(TObjectPtr<UAnimSequence> InIdleSequence, TObjectPtr<UAnimMontage> InObjectMontage)
{
	if(InIdleSequence)
	{
		ObjectIdleAnimation = InIdleSequence;
	}
	if(InObjectMontage)
	{
		ObjectMontage = InObjectMontage;
	}	
}
