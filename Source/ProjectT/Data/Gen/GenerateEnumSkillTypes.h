//Generated SkillTypes Index to Enum

#pragma once

UENUM(BlueprintType)
enum class ESkillTypes : uint8 
{
	None = 0 UMETA(DisplayName = "None"),
	Melee UMETA(DisplayName= "Melee"),
	Direct UMETA(DisplayName= "Direct"),
	Indirect UMETA(DisplayName= "Indirect"),
	Buff UMETA(DisplayName= "Buff"),
};
ENUM_RANGE_BY_FIRST_AND_LAST(ESkillTypes,ESkillTypes::Melee,ESkillTypes::Buff);

static FString EnumToString(ESkillTypes InEnum)
{
	switch (InEnum)
	{
		case ESkillTypes::Melee: return TEXT("Melee");
		case ESkillTypes::Direct: return TEXT("Direct");
		case ESkillTypes::Indirect: return TEXT("Indirect");
		case ESkillTypes::Buff: return TEXT("Buff");
		default: checkf(false, TEXT("Error"));
	}
	return FString();
}
