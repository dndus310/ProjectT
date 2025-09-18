#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GenerateStructCharacterUnlock.generated.h"

USTRUCT(BlueprintType)
struct FCharacterUnlock : public FTableRowBase 
{
	GENERATED_BODY()

	FCharacterUnlock() :
		sUnlockType(),
		iPlayerCharId(),
		iClearStage(),
		iUnlockDescId()
	{
	}

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = CharacterUnlock)
	FString sUnlockType;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = CharacterUnlock)
	int32 iPlayerCharId;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = CharacterUnlock)
	int32 iClearStage;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = CharacterUnlock)
	int32 iUnlockDescId;

	bool operator==(const FCharacterUnlock& Other) const
	{
		return sUnlockType == Other.sUnlockType && iPlayerCharId == Other.iPlayerCharId && iClearStage == Other.iClearStage && iUnlockDescId == Other.iUnlockDescId;
	}
	bool operator!=(const FCharacterUnlock& Other) const
	{
		return !(*this == Other);
	}
	bool IsDefault() const
	{
		return sUnlockType.IsEmpty() && iPlayerCharId == 0 && iClearStage == 0 && iUnlockDescId == 0;
	}
};

