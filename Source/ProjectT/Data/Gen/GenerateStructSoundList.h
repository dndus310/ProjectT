#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GenerateStructSoundList.generated.h"

USTRUCT(BlueprintType)
struct FSoundList : public FTableRowBase 
{
	GENERATED_BODY()

	FSoundList() :
		sSoundClass(),
		sSoundType(),
		sSoundPath(),
		sAttenPath(),
		bLooping()
	{
	}

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = SoundList)
	FString sSoundClass;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = SoundList)
	FString sSoundType;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = SoundList)
	FString sSoundPath;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = SoundList)
	FString sAttenPath;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = SoundList)
	bool bLooping;

	bool operator==(const FSoundList& Other) const
	{
		return sSoundClass == Other.sSoundClass && sSoundType == Other.sSoundType && sSoundPath == Other.sSoundPath && sAttenPath == Other.sAttenPath && bLooping == Other.bLooping;
	}
	bool operator!=(const FSoundList& Other) const
	{
		return !(*this == Other);
	}
	bool IsDefault() const
	{
		return sSoundClass.IsEmpty() && sSoundType.IsEmpty() && sSoundPath.IsEmpty() && sAttenPath.IsEmpty() && bLooping == 0;
	}
};

