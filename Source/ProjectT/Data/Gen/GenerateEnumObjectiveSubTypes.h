//Generated ObjectiveSubTypes Index to Enum

#pragma once

UENUM(BlueprintType)
enum class EObjectiveSubTypes : uint8 
{
	None = 0 UMETA(DisplayName = "None"),
	Save UMETA(DisplayName= "Save"),
	Collect UMETA(DisplayName= "Collect"),
	Avoid UMETA(DisplayName= "Avoid"),
	Kill UMETA(DisplayName= "Kill"),
};
ENUM_RANGE_BY_FIRST_AND_LAST(EObjectiveSubTypes,EObjectiveSubTypes::Save,EObjectiveSubTypes::Kill);

static FString EnumToString(EObjectiveSubTypes InEnum)
{
	switch (InEnum)
	{
		case EObjectiveSubTypes::Save: return TEXT("Save");
		case EObjectiveSubTypes::Collect: return TEXT("Collect");
		case EObjectiveSubTypes::Avoid: return TEXT("Avoid");
		case EObjectiveSubTypes::Kill: return TEXT("Kill");
		default: checkf(false, TEXT("Error"));
	}
	return FString();
}
