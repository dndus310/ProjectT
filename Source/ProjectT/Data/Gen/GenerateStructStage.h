#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GenerateStructStage.generated.h"

USTRUCT(BlueprintType)
struct FStage : public FTableRowBase 
{
	GENERATED_BODY()

	FStage() :
		iMissionId(),
		iQuest1Id(),
		iQuest2Id(),
		iQuest3Id(),
		iTimeLimit(),
		iNextStageId(),
		sStageName(),
		sStagePath(),
		sStageThumbnailPath(),
		sStageBackgroundPath(),
		iStageNameId(),
		iStageDescId()
	{
	}

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Stage)
	int32 iMissionId;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Stage)
	int32 iQuest1Id;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Stage)
	int32 iQuest2Id;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Stage)
	int32 iQuest3Id;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Stage)
	int32 iTimeLimit;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Stage)
	int32 iNextStageId;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Stage)
	FString sStageName;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Stage)
	FString sStagePath;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Stage)
	FString sStageThumbnailPath;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Stage)
	FString sStageBackgroundPath;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Stage)
	int32 iStageNameId;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Stage)
	int32 iStageDescId;

	bool operator==(const FStage& Other) const
	{
		return iMissionId == Other.iMissionId && iQuest1Id == Other.iQuest1Id && iQuest2Id == Other.iQuest2Id && iQuest3Id == Other.iQuest3Id && iTimeLimit == Other.iTimeLimit && iNextStageId == Other.iNextStageId && sStageName == Other.sStageName && sStagePath == Other.sStagePath && sStageThumbnailPath == Other.sStageThumbnailPath && sStageBackgroundPath == Other.sStageBackgroundPath && iStageNameId == Other.iStageNameId && iStageDescId == Other.iStageDescId;
	}
	bool operator!=(const FStage& Other) const
	{
		return !(*this == Other);
	}
	bool IsDefault() const
	{
		return iMissionId == 0 && iQuest1Id == 0 && iQuest2Id == 0 && iQuest3Id == 0 && iTimeLimit == 0 && iNextStageId == 0 && sStageName.IsEmpty() && sStagePath.IsEmpty() && sStageThumbnailPath.IsEmpty() && sStageBackgroundPath.IsEmpty() && iStageNameId == 0 && iStageDescId == 0;
	}
};

