#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GenerateStructItemSet.generated.h"

USTRUCT(BlueprintType)
struct FItemSet : public FTableRowBase 
{
	GENERATED_BODY()

	FItemSet() :
		iItemGroupId(),
		iItemId(),
		iProbWeight()
	{
	}

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = ItemSet)
	int32 iItemGroupId;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = ItemSet)
	int32 iItemId;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = ItemSet)
	int32 iProbWeight;

	bool operator==(const FItemSet& Other) const
	{
		return iItemGroupId == Other.iItemGroupId && iItemId == Other.iItemId && iProbWeight == Other.iProbWeight;
	}
	bool operator!=(const FItemSet& Other) const
	{
		return !(*this == Other);
	}
	bool IsDefault() const
	{
		return iItemGroupId == 0 && iItemId == 0 && iProbWeight == 0;
	}
};

