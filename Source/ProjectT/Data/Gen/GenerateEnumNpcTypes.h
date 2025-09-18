//Generated NpcTypes Index to Enum

#pragma once

UENUM(BlueprintType)
enum class ENpcTypes : uint8 
{
	None = 0 UMETA(DisplayName = "None"),
	Stationary UMETA(DisplayName= "Stationary"),
	Patrol UMETA(DisplayName= "Patrol"),
	Fixed UMETA(DisplayName= "Fixed"),
};
ENUM_RANGE_BY_FIRST_AND_LAST(ENpcTypes,ENpcTypes::Stationary,ENpcTypes::Fixed);

static FString EnumToString(ENpcTypes InEnum)
{
	switch (InEnum)
	{
		case ENpcTypes::Stationary: return TEXT("Stationary");
		case ENpcTypes::Patrol: return TEXT("Patrol");
		case ENpcTypes::Fixed: return TEXT("Fixed");
		default: checkf(false, TEXT("Error"));
	}
	return FString();
}
