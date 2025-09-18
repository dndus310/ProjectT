// Fill out your copyright notice in the Description page of Project Settings.


#include "NpcAnimInstance.h"


UNpcAnimInstance::UNpcAnimInstance() :
	ForwardHeadRotation(FRotator(0.0, -90.0, 21.0)),
	HeadRotation(ForwardHeadRotation),
	HeadRotAlpha(0.f)
{
}

void UNpcAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
}

void UNpcAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
}
