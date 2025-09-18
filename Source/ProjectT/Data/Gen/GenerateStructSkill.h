#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GenerateStructSkill.generated.h"

USTRUCT(BlueprintType)
struct FSkill : public FTableRowBase 
{
	GENERATED_BODY()

	FSkill() :
		sSkillName(),
		sSkillType(),
		sSkillIconPath(),
		fCooldown(),
		fDamage(),
		fCriDmg(),
		fSkillNoise(),
		fCorrectRange(),
		fSkillRange(),
		fProjectileSpeed(),
		fEffectRange(),
		iEffect1Id(),
		iEffect2Id(),
		iEffect3Id(),
		sSkillBpPath(),
		iSkillSocketId(),
		iSkillNameId(),
		iSkillDescId()
	{
	}

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Skill)
	FString sSkillName;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Skill)
	FString sSkillType;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Skill)
	FString sSkillIconPath;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Skill)
	float fCooldown;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Skill)
	float fDamage;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Skill)
	float fCriDmg;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Skill)
	float fSkillNoise;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Skill)
	float fCorrectRange;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Skill)
	float fSkillRange;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Skill)
	float fProjectileSpeed;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Skill)
	float fEffectRange;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Skill)
	int32 iEffect1Id;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Skill)
	int32 iEffect2Id;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Skill)
	int32 iEffect3Id;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Skill)
	FString sSkillBpPath;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Skill)
	int32 iSkillSocketId;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Skill)
	int32 iSkillNameId;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Skill)
	int32 iSkillDescId;

	bool operator==(const FSkill& Other) const
	{
		return sSkillName == Other.sSkillName && sSkillType == Other.sSkillType && sSkillIconPath == Other.sSkillIconPath && fCooldown == Other.fCooldown && fDamage == Other.fDamage && fCriDmg == Other.fCriDmg && fSkillNoise == Other.fSkillNoise && fCorrectRange == Other.fCorrectRange && fSkillRange == Other.fSkillRange && fProjectileSpeed == Other.fProjectileSpeed && fEffectRange == Other.fEffectRange && iEffect1Id == Other.iEffect1Id && iEffect2Id == Other.iEffect2Id && iEffect3Id == Other.iEffect3Id && sSkillBpPath == Other.sSkillBpPath && iSkillSocketId == Other.iSkillSocketId && iSkillNameId == Other.iSkillNameId && iSkillDescId == Other.iSkillDescId;
	}
	bool operator!=(const FSkill& Other) const
	{
		return !(*this == Other);
	}
	bool IsDefault() const
	{
		return sSkillName.IsEmpty() && sSkillType.IsEmpty() && sSkillIconPath.IsEmpty() && fCooldown == 0 && fDamage == 0 && fCriDmg == 0 && fSkillNoise == 0 && fCorrectRange == 0 && fSkillRange == 0 && fProjectileSpeed == 0 && fEffectRange == 0 && iEffect1Id == 0 && iEffect2Id == 0 && iEffect3Id == 0 && sSkillBpPath.IsEmpty() && iSkillSocketId == 0 && iSkillNameId == 0 && iSkillDescId == 0;
	}
};

