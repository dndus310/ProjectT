#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class ELoadingPhase : uint8
{
	None,
	MapLoading,
	ShaderCompiling,
	Finished
};
