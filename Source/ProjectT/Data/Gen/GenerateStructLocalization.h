#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GenerateStructLocalization.generated.h"

USTRUCT(BlueprintType)
struct FLocalization : public FTableRowBase 
{
	GENERATED_BODY()

	FLocalization() :
		sKorean(),
		sEnglish()
	{
	}

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Localization)
	FString sKorean;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Localization)
	FString sEnglish;

	bool operator==(const FLocalization& Other) const
	{
		return sKorean == Other.sKorean && sEnglish == Other.sEnglish;
	}
	bool operator!=(const FLocalization& Other) const
	{
		return !(*this == Other);
	}
	bool IsDefault() const
	{
		return sKorean.IsEmpty() && sEnglish.IsEmpty();
	}
};

