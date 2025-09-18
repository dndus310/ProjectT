#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GenerateStructLevelObject.generated.h"

USTRUCT(BlueprintType)
struct FLevelObject : public FTableRowBase 
{
	GENERATED_BODY()

	FLevelObject() :
		sLevelObjType(),
		iCastItemId()
	{
	}

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = LevelObject)
	FString sLevelObjType;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = LevelObject)
	int32 iCastItemId;

	bool operator==(const FLevelObject& Other) const
	{
		return sLevelObjType == Other.sLevelObjType && iCastItemId == Other.iCastItemId;
	}
	bool operator!=(const FLevelObject& Other) const
	{
		return !(*this == Other);
	}
	bool IsDefault() const
	{
		return sLevelObjType.IsEmpty() && iCastItemId == 0;
	}
};

