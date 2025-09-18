#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GenerateStructSocketList.generated.h"

USTRUCT(BlueprintType)
struct FSocketList : public FTableRowBase 
{
	GENERATED_BODY()

	FSocketList() :
		sSocketName()
	{
	}

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = SocketList)
	FString sSocketName;

	bool operator==(const FSocketList& Other) const
	{
		return sSocketName == Other.sSocketName;
	}
	bool operator!=(const FSocketList& Other) const
	{
		return !(*this == Other);
	}
	bool IsDefault() const
	{
		return sSocketName.IsEmpty();
	}
};

