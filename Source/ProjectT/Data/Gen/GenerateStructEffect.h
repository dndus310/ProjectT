#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GenerateStructEffect.generated.h"

USTRUCT(BlueprintType)
struct FEffect : public FTableRowBase 
{
	GENERATED_BODY()

	FEffect() :
		sEffectType(),
		sEffect(),
		fEffectValue(),
		fEffectDuration(),
		iEffectNameId(),
		iEffectDescId(),
		sEffectIconPath()
	{
	}

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Effect)
	FString sEffectType;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Effect)
	FString sEffect;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Effect)
	float fEffectValue;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Effect)
	float fEffectDuration;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Effect)
	int32 iEffectNameId;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Effect)
	int32 iEffectDescId;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Effect)
	FString sEffectIconPath;

	bool operator==(const FEffect& Other) const
	{
		return sEffectType == Other.sEffectType && sEffect == Other.sEffect && fEffectValue == Other.fEffectValue && fEffectDuration == Other.fEffectDuration && iEffectNameId == Other.iEffectNameId && iEffectDescId == Other.iEffectDescId && sEffectIconPath == Other.sEffectIconPath;
	}
	bool operator!=(const FEffect& Other) const
	{
		return !(*this == Other);
	}
	bool IsDefault() const
	{
		return sEffectType.IsEmpty() && sEffect.IsEmpty() && fEffectValue == 0 && fEffectDuration == 0 && iEffectNameId == 0 && iEffectDescId == 0 && sEffectIconPath.IsEmpty();
	}
};

