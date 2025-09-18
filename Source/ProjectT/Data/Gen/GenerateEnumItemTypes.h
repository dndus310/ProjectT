//Generated ItemTypes Index to Enum

#pragma once

UENUM(BlueprintType)
enum class EItemTypes : uint8 
{
	None = 0 UMETA(DisplayName = "None"),
	Gimick UMETA(DisplayName= "Gimick"),
	Usable UMETA(DisplayName= "Usable"),
	Collection UMETA(DisplayName= "Collection"),
};
ENUM_RANGE_BY_FIRST_AND_LAST(EItemTypes,EItemTypes::Gimick,EItemTypes::Collection);

static FString EnumToString(EItemTypes InEnum)
{
	switch (InEnum)
	{
		case EItemTypes::Gimick: return TEXT("Gimick");
		case EItemTypes::Usable: return TEXT("Usable");
		case EItemTypes::Collection: return TEXT("Collection");
		default: checkf(false, TEXT("Error"));
	}
	return FString();
}
