#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GenerateStructPlayerCharacter.generated.h"

USTRUCT(BlueprintType)
struct FPlayerCharacter : public FTableRowBase 
{
	GENERATED_BODY()

	FPlayerCharacter() :
		fCharHp(),
		fCharMove(),
		fCharMoveSound(),
		fCharRun(),
		fCharRunSound(),
		fStamina(),
		fStaRecovery(),
		iItemSlot(),
		iCharAttackId(),
		iCharSkillId(),
		iCharNameId(),
		iCharDescId(),
		iPassiveName1Id(),
		iPassiveDesc1Id(),
		iPassiveName2Id(),
		iPassiveDesc2Id(),
		iPassiveName3Id(),
		iPassiveDesc3Id(),
		iCharMeshSetId(),
		iCharAniSetId(),
		sCharBlueprintPath(),
		sCharIconPath()
	{
	}

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = PlayerCharacter)
	float fCharHp;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = PlayerCharacter)
	float fCharMove;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = PlayerCharacter)
	float fCharMoveSound;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = PlayerCharacter)
	float fCharRun;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = PlayerCharacter)
	float fCharRunSound;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = PlayerCharacter)
	float fStamina;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = PlayerCharacter)
	float fStaRecovery;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = PlayerCharacter)
	int32 iItemSlot;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = PlayerCharacter)
	int32 iCharAttackId;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = PlayerCharacter)
	int32 iCharSkillId;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = PlayerCharacter)
	int32 iCharNameId;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = PlayerCharacter)
	int32 iCharDescId;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = PlayerCharacter)
	int32 iPassiveName1Id;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = PlayerCharacter)
	int32 iPassiveDesc1Id;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = PlayerCharacter)
	int32 iPassiveName2Id;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = PlayerCharacter)
	int32 iPassiveDesc2Id;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = PlayerCharacter)
	int32 iPassiveName3Id;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = PlayerCharacter)
	int32 iPassiveDesc3Id;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = PlayerCharacter)
	int32 iCharMeshSetId;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = PlayerCharacter)
	int32 iCharAniSetId;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = PlayerCharacter)
	FString sCharBlueprintPath;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = PlayerCharacter)
	FString sCharIconPath;

	bool operator==(const FPlayerCharacter& Other) const
	{
		return fCharHp == Other.fCharHp && fCharMove == Other.fCharMove && fCharMoveSound == Other.fCharMoveSound && fCharRun == Other.fCharRun && fCharRunSound == Other.fCharRunSound && fStamina == Other.fStamina && fStaRecovery == Other.fStaRecovery && iItemSlot == Other.iItemSlot && iCharAttackId == Other.iCharAttackId && iCharSkillId == Other.iCharSkillId && iCharNameId == Other.iCharNameId && iCharDescId == Other.iCharDescId && iPassiveName1Id == Other.iPassiveName1Id && iPassiveDesc1Id == Other.iPassiveDesc1Id && iPassiveName2Id == Other.iPassiveName2Id && iPassiveDesc2Id == Other.iPassiveDesc2Id && iPassiveName3Id == Other.iPassiveName3Id && iPassiveDesc3Id == Other.iPassiveDesc3Id && iCharMeshSetId == Other.iCharMeshSetId && iCharAniSetId == Other.iCharAniSetId && sCharBlueprintPath == Other.sCharBlueprintPath && sCharIconPath == Other.sCharIconPath;
	}
	bool operator!=(const FPlayerCharacter& Other) const
	{
		return !(*this == Other);
	}
	bool IsDefault() const
	{
		return fCharHp == 0 && fCharMove == 0 && fCharMoveSound == 0 && fCharRun == 0 && fCharRunSound == 0 && fStamina == 0 && fStaRecovery == 0 && iItemSlot == 0 && iCharAttackId == 0 && iCharSkillId == 0 && iCharNameId == 0 && iCharDescId == 0 && iPassiveName1Id == 0 && iPassiveDesc1Id == 0 && iPassiveName2Id == 0 && iPassiveDesc2Id == 0 && iPassiveName3Id == 0 && iPassiveDesc3Id == 0 && iCharMeshSetId == 0 && iCharAniSetId == 0 && sCharBlueprintPath.IsEmpty() && sCharIconPath.IsEmpty();
	}
};

