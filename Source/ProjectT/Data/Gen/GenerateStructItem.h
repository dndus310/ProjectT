#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GenerateStructItem.generated.h"

USTRUCT(BlueprintType)
struct FItem : public FTableRowBase 
{
	GENERATED_BODY()

	FItem() :
		sItemType(),
		iItemSkillId(),
		iItemMeshId(),
		sItemIconPath(),
		iItemNameId(),
		iItemDescId()
	{
	}

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Item)
	FString sItemType;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Item)
	int32 iItemSkillId;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Item)
	int32 iItemMeshId;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Item)
	FString sItemIconPath;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Item)
	int32 iItemNameId;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Item)
	int32 iItemDescId;

	bool operator==(const FItem& Other) const
	{
		return sItemType == Other.sItemType && iItemSkillId == Other.iItemSkillId && iItemMeshId == Other.iItemMeshId && sItemIconPath == Other.sItemIconPath && iItemNameId == Other.iItemNameId && iItemDescId == Other.iItemDescId;
	}
	bool operator!=(const FItem& Other) const
	{
		return !(*this == Other);
	}
	bool IsDefault() const
	{
		return sItemType.IsEmpty() && iItemSkillId == 0 && iItemMeshId == 0 && sItemIconPath.IsEmpty() && iItemNameId == 0 && iItemDescId == 0;
	}
};

