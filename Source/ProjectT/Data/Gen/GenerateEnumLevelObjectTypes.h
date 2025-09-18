//Generated LevelObjectTypes Index to Enum

#pragma once

UENUM(BlueprintType)
enum class ELevelObjectTypes : uint8 
{
	None = 0 UMETA(DisplayName = "None"),
	Cast UMETA(DisplayName= "Cast"),
	Trap UMETA(DisplayName= "Trap"),
};
ENUM_RANGE_BY_FIRST_AND_LAST(ELevelObjectTypes,ELevelObjectTypes::Cast,ELevelObjectTypes::Trap);

static FString EnumToString(ELevelObjectTypes InEnum)
{
	switch (InEnum)
	{
		case ELevelObjectTypes::Cast: return TEXT("Cast");
		case ELevelObjectTypes::Trap: return TEXT("Trap");
		default: checkf(false, TEXT("Error"));
	}
	return FString();
}
