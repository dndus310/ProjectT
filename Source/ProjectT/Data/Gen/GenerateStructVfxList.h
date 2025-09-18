#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GenerateStructVfxList.generated.h"

USTRUCT(BlueprintType)
struct FVfxList : public FTableRowBase 
{
	GENERATED_BODY()

	FVfxList() :
		sVfxPath()
	{
	}

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = VfxList)
	FString sVfxPath;

	bool operator==(const FVfxList& Other) const
	{
		return sVfxPath == Other.sVfxPath;
	}
	bool operator!=(const FVfxList& Other) const
	{
		return !(*this == Other);
	}
	bool IsDefault() const
	{
		return sVfxPath.IsEmpty();
	}
};

