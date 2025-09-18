// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "UserDataModel.generated.h"


USTRUCT()
struct FSubQuestData
{
	GENERATED_BODY()

	FSubQuestData() {}
	bool operator==(const FUserSaveData& Other) const
	{
		return (*this == Other);
	}
	bool operator!=(const FUserSaveData& Other) const
	{
		return !(*this == Other);
	}
	bool IsDefault() const
	{
		return true;
	}
};

USTRUCT(BlueprintType)
struct FUnlockCharacterData
{
	GENERATED_BODY()

	FUnlockCharacterData() : bCleared(false) {}
	FUnlockCharacterData(const FString& InRowName, bool InCleared = false) :
		UnlockCharacterRowName(InRowName), bCleared(InCleared) {
	}

	bool operator==(const FUserSaveData& Other) const
	{
		return (*this == Other);
	}
	bool operator!=(const FUserSaveData& Other) const
	{
		return !(*this == Other);
	}
	bool IsDefault() const
	{
		return UnlockCharacterRowName.IsEmpty() && !bCleared;
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString UnlockCharacterRowName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCleared;
};

USTRUCT(BlueprintType)
struct FStageData
{
	GENERATED_BODY()

	FStageData() : bCleared(false) {}
	FStageData(const FString& InRowName, bool InCleared = false) :
		StageRowName(InRowName), bCleared(InCleared) {
	}

	bool operator==(const FUserSaveData& Other) const
	{
		return (*this == Other);
	}
	bool operator!=(const FUserSaveData& Other) const
	{
		return !(*this == Other);
	}
	bool IsDefault() const
	{
		return StageRowName.IsEmpty() && !bCleared;
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString StageRowName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCleared;
};

USTRUCT()
struct FUserSaveData
{
	GENERATED_BODY()

	FUserSaveData()
	{
		if(UnlockCharacterData.IsEmpty())
			UnlockCharacterData.Push(FUnlockCharacterData(TEXT("200001"), true));

		if(UnlockStageData.IsEmpty())
			UnlockStageData.Push(FStageData(TEXT("100101"), true));
	}
	bool operator==(const FUserSaveData& Other) const
	{
		return (*this == Other);
	}
	bool operator!=(const FUserSaveData& Other) const
	{
		return !(*this == Other);
	}
	bool IsDefault() const
	{
		return SubQuestData.IsDefault() && UnlockStageData.IsEmpty() && UnlockCharacterData.IsEmpty();
	}

	UPROPERTY()
	FSubQuestData SubQuestData;

	UPROPERTY()
	TArray<FStageData> UnlockStageData;

	UPROPERTY()
	TArray<FUnlockCharacterData> UnlockCharacterData;

	UPROPERTY()
	FString SelectedCharacterData;
};


UCLASS()
class PROJECTT_API UUserDataModel : public UObject
{
	GENERATED_BODY()
};
