//Generated ObjectiveTypes Index to Enum

#pragma once

UENUM(BlueprintType)
enum class EObjectiveTypes : uint8 
{
	None = 0 UMETA(DisplayName = "None"),
	Mission UMETA(DisplayName= "Mission"),
	Quest UMETA(DisplayName= "Quest"),
};
ENUM_RANGE_BY_FIRST_AND_LAST(EObjectiveTypes,EObjectiveTypes::Mission,EObjectiveTypes::Quest);

static FString EnumToString(EObjectiveTypes InEnum)
{
	switch (InEnum)
	{
		case EObjectiveTypes::Mission: return TEXT("Mission");
		case EObjectiveTypes::Quest: return TEXT("Quest");
		default: checkf(false, TEXT("Error"));
	}
	return FString();
}
