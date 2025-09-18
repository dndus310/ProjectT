//Generated EffectSubTypes Index to Enum

#pragma once

UENUM(BlueprintType)
enum class EEffectSubTypes : uint8 
{
	None = 0 UMETA(DisplayName = "None"),
	Hitback UMETA(DisplayName= "Hitback"),
	Jumpback UMETA(DisplayName= "Jumpback"),
	Stun UMETA(DisplayName= "Stun"),
	Bind UMETA(DisplayName= "Bind"),
	Surge UMETA(DisplayName= "Surge"),
	Unrecog UMETA(DisplayName= "Unrecog"),
	CogImmune UMETA(DisplayName= "CogImmune"),
	HitbackImmune UMETA(DisplayName= "HitbackImmune"),
	Fast UMETA(DisplayName= "Fast"),
	Slow UMETA(DisplayName= "Slow"),
	Heal UMETA(DisplayName= "Heal"),
};
ENUM_RANGE_BY_FIRST_AND_LAST(EEffectSubTypes,EEffectSubTypes::Hitback,EEffectSubTypes::Heal);

static FString EnumToString(EEffectSubTypes InEnum)
{
	switch (InEnum)
	{
		case EEffectSubTypes::Hitback: return TEXT("Hitback");
		case EEffectSubTypes::Jumpback: return TEXT("Jumpback");
		case EEffectSubTypes::Stun: return TEXT("Stun");
		case EEffectSubTypes::Bind: return TEXT("Bind");
		case EEffectSubTypes::Surge: return TEXT("Surge");
		case EEffectSubTypes::Unrecog: return TEXT("Unrecog");
		case EEffectSubTypes::CogImmune: return TEXT("CogImmune");
		case EEffectSubTypes::HitbackImmune: return TEXT("HitbackImmune");
		case EEffectSubTypes::Fast: return TEXT("Fast");
		case EEffectSubTypes::Slow: return TEXT("Slow");
		case EEffectSubTypes::Heal: return TEXT("Heal");
		default: checkf(false, TEXT("Error"));
	}
	return FString();
}
