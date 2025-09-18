#pragma once

#include "CoreMinimal.h"
#include "InputTriggers.h"
#include "KeyTrigger.generated.h"

UCLASS()
class PROJECTT_API UKeyTrigger : public UInputTrigger
{
	GENERATED_BODY()


public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = InputTrigger)
	TArray<FKey> AdditionalKeys;

protected:
	virtual ETriggerType GetTriggerType_Implementation() const override { return ETriggerType::Implicit;}
	virtual ETriggerState UpdateState_Implementation(const UEnhancedPlayerInput* PlayerInput, FInputActionValue ModifiedValue, float DeltaTime) override;
	
};
