#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "ObjectiveCharacterAnimInstance.generated.h"

UCLASS()
class PROJECTT_API UObjectiveCharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UObjectiveCharacterAnimInstance();
	
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
};
