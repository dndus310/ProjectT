#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GenerateStructCharacterMeshSet.generated.h"

USTRUCT(BlueprintType)
struct FCharacterMeshSet : public FTableRowBase 
{
	GENERATED_BODY()

	FCharacterMeshSet() :
		iBodyMeshId(),
		iWeaponMeshId(),
		iWeaponSocketId()
	{
	}

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = CharacterMeshSet)
	int32 iBodyMeshId;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = CharacterMeshSet)
	int32 iWeaponMeshId;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = CharacterMeshSet)
	int32 iWeaponSocketId;

	bool operator==(const FCharacterMeshSet& Other) const
	{
		return iBodyMeshId == Other.iBodyMeshId && iWeaponMeshId == Other.iWeaponMeshId && iWeaponSocketId == Other.iWeaponSocketId;
	}
	bool operator!=(const FCharacterMeshSet& Other) const
	{
		return !(*this == Other);
	}
	bool IsDefault() const
	{
		return iBodyMeshId == 0 && iWeaponMeshId == 0 && iWeaponSocketId == 0;
	}
};

