//Generated UnlockTypes Index to Enum

#pragma once

UENUM(BlueprintType)
enum class EUnlockTypes : uint8 
{
	None = 0 UMETA(DisplayName = "None"),
	Default UMETA(DisplayName= "Default"),
	Mission UMETA(DisplayName= "Mission"),
	Quest UMETA(DisplayName= "Quest"),
};
ENUM_RANGE_BY_FIRST_AND_LAST(EUnlockTypes,EUnlockTypes::Default,EUnlockTypes::Quest);

static FString EnumToString(EUnlockTypes InEnum)
{
	switch (InEnum)
	{
		case EUnlockTypes::Default: return TEXT("Default");
		case EUnlockTypes::Mission: return TEXT("Mission");
		case EUnlockTypes::Quest: return TEXT("Quest");
		default: checkf(false, TEXT("Error"));
	}
	return FString();
}
