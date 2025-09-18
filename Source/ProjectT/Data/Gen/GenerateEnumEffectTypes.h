//Generated EffectTypes Index to Enum

#pragma once

UENUM(BlueprintType)
enum class EEffectTypes : uint8 
{
	None = 0 UMETA(DisplayName = "None"),
	CC UMETA(DisplayName= "CC"),
	Utility UMETA(DisplayName= "Utility"),
	Status UMETA(DisplayName= "Status"),
};
ENUM_RANGE_BY_FIRST_AND_LAST(EEffectTypes,EEffectTypes::CC,EEffectTypes::Status);

static FString EnumToString(EEffectTypes InEnum)
{
	switch (InEnum)
	{
		case EEffectTypes::CC: return TEXT("CC");
		case EEffectTypes::Utility: return TEXT("Utility");
		case EEffectTypes::Status: return TEXT("Status");
		default: checkf(false, TEXT("Error"));
	}
	return FString();
}
