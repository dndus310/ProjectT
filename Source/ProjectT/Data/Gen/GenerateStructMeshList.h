#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GenerateStructMeshList.generated.h"

USTRUCT(BlueprintType)
struct FMeshList : public FTableRowBase 
{
	GENERATED_BODY()

	FMeshList() :
		sMeshPath()
	{
	}

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = MeshList)
	FString sMeshPath;

	bool operator==(const FMeshList& Other) const
	{
		return sMeshPath == Other.sMeshPath;
	}
	bool operator!=(const FMeshList& Other) const
	{
		return !(*this == Other);
	}
	bool IsDefault() const
	{
		return sMeshPath.IsEmpty();
	}
};

