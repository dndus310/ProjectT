#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GenerateStructEnemyNpc.generated.h"

USTRUCT(BlueprintType)
struct FEnemyNpc : public FTableRowBase 
{
	GENERATED_BODY()

	FEnemyNpc() :
		sNpcType(),
		fNpcHp(),
		fNpcMove(),
		fNpcRun(),
		fSightLength(),
		fSightAngle(),
		fAttackLength(),
		fChaseLength(),
		fRotationAngle(),
		fRotationSpeed(),
		fDoubtTime(),
		fNpcNoise(),
		fMoveProb(),
		fSearchProb(),
		fScoutProb(),
		iNpcAttackId(),
		iNpcNameId(),
		iNpcMeshSetId()
	{
	}

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = EnemyNpc)
	FString sNpcType;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = EnemyNpc)
	float fNpcHp;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = EnemyNpc)
	float fNpcMove;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = EnemyNpc)
	float fNpcRun;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = EnemyNpc)
	float fSightLength;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = EnemyNpc)
	float fSightAngle;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = EnemyNpc)
	float fAttackLength;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = EnemyNpc)
	float fChaseLength;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = EnemyNpc)
	float fRotationAngle;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = EnemyNpc)
	float fRotationSpeed;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = EnemyNpc)
	float fDoubtTime;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = EnemyNpc)
	float fNpcNoise;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = EnemyNpc)
	float fMoveProb;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = EnemyNpc)
	float fSearchProb;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = EnemyNpc)
	float fScoutProb;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = EnemyNpc)
	int32 iNpcAttackId;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = EnemyNpc)
	int32 iNpcNameId;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = EnemyNpc)
	int32 iNpcMeshSetId;

	bool operator==(const FEnemyNpc& Other) const
	{
		return sNpcType == Other.sNpcType && fNpcHp == Other.fNpcHp && fNpcMove == Other.fNpcMove && fNpcRun == Other.fNpcRun && fSightLength == Other.fSightLength && fSightAngle == Other.fSightAngle && fAttackLength == Other.fAttackLength && fChaseLength == Other.fChaseLength && fRotationAngle == Other.fRotationAngle && fRotationSpeed == Other.fRotationSpeed && fDoubtTime == Other.fDoubtTime && fNpcNoise == Other.fNpcNoise && fMoveProb == Other.fMoveProb && fSearchProb == Other.fSearchProb && fScoutProb == Other.fScoutProb && iNpcAttackId == Other.iNpcAttackId && iNpcNameId == Other.iNpcNameId && iNpcMeshSetId == Other.iNpcMeshSetId;
	}
	bool operator!=(const FEnemyNpc& Other) const
	{
		return !(*this == Other);
	}
	bool IsDefault() const
	{
		return sNpcType.IsEmpty() && fNpcHp == 0 && fNpcMove == 0 && fNpcRun == 0 && fSightLength == 0 && fSightAngle == 0 && fAttackLength == 0 && fChaseLength == 0 && fRotationAngle == 0 && fRotationSpeed == 0 && fDoubtTime == 0 && fNpcNoise == 0 && fMoveProb == 0 && fSearchProb == 0 && fScoutProb == 0 && iNpcAttackId == 0 && iNpcNameId == 0 && iNpcMeshSetId == 0;
	}
};

