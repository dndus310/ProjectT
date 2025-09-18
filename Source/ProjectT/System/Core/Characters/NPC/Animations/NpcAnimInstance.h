// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NpcAnimInstance.generated.h"


UCLASS()
class PROJECTT_API UNpcAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UNpcAnimInstance();
	
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FRotator ForwardHeadRotation;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FRotator HeadRotation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float HeadRotAlpha;
};



