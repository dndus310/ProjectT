#include "ProjectT/System/Core/Input/KeyTrigger.h"
#include "EnhancedPlayerInput.h"

ETriggerState UKeyTrigger::UpdateState_Implementation(const UEnhancedPlayerInput* PlayerInput,
	FInputActionValue ModifiedValue, float DeltaTime)
{
	ETriggerState ParentState = Super::UpdateState_Implementation(PlayerInput, ModifiedValue, DeltaTime);

	for (const FKey& Key : AdditionalKeys)
	{
		if (PlayerInput->IsPressed(Key) == false) return ETriggerState::Ongoing; 
	}

	return ParentState;
}
