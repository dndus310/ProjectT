// Fill out your copyright notice in the Description page of Project Settings.


#include "NpcFixedAnimInstance.h"

UNpcFixedAnimInstance::UNpcFixedAnimInstance() 
{
	ForwardHeadRotation = FRotator(0.0, 0.0, 90.0);
	HeadRotation = ForwardHeadRotation;
	HeadRotAlpha = 0.f;
}
