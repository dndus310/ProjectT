#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GenerateStructAnimationSet.generated.h"

USTRUCT(BlueprintType)
struct FAnimationSet : public FTableRowBase 
{
	GENERATED_BODY()

	FAnimationSet() :
		sIdleAniPath()
	{
	}

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = AnimationSet)
	FString sIdleAniPath;

	bool operator==(const FAnimationSet& Other) const
	{
		return sIdleAniPath == Other.sIdleAniPath;
	}
	bool operator!=(const FAnimationSet& Other) const
	{
		return !(*this == Other);
	}
	bool IsDefault() const
	{
		return sIdleAniPath.IsEmpty();
	}
};

