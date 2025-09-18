#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GenerateStructHapticList.generated.h"

USTRUCT(BlueprintType)
struct FHapticList : public FTableRowBase 
{
	GENERATED_BODY()

	FHapticList() :
		sHapticPath()
	{
	}

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = HapticList)
	FString sHapticPath;

	bool operator==(const FHapticList& Other) const
	{
		return sHapticPath == Other.sHapticPath;
	}
	bool operator!=(const FHapticList& Other) const
	{
		return !(*this == Other);
	}
	bool IsDefault() const
	{
		return sHapticPath.IsEmpty();
	}
};

