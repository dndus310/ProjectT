#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GenerateStructObjective.generated.h"

USTRUCT(BlueprintType)
struct FObjective : public FTableRowBase 
{
	GENERATED_BODY()

	FObjective() :
		sObjectiveType(),
		sObjectiveSubType(),
		iObjectiveItemId(),
		iObjectiveCount(),
		iObjectiveDescId()
	{
	}

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Objective)
	FString sObjectiveType;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Objective)
	FString sObjectiveSubType;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Objective)
	int32 iObjectiveItemId;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Objective)
	int32 iObjectiveCount;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Objective)
	int32 iObjectiveDescId;

	bool operator==(const FObjective& Other) const
	{
		return sObjectiveType == Other.sObjectiveType && sObjectiveSubType == Other.sObjectiveSubType && iObjectiveItemId == Other.iObjectiveItemId && iObjectiveCount == Other.iObjectiveCount && iObjectiveDescId == Other.iObjectiveDescId;
	}
	bool operator!=(const FObjective& Other) const
	{
		return !(*this == Other);
	}
	bool IsDefault() const
	{
		return sObjectiveType.IsEmpty() && sObjectiveSubType.IsEmpty() && iObjectiveItemId == 0 && iObjectiveCount == 0 && iObjectiveDescId == 0;
	}
};

