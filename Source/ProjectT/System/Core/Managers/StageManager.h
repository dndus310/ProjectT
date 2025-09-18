// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectT/Data/Gen/GenerateStructStage.h"
#include "UObject/Object.h"
#include "StageManager.generated.h"

struct FStage;
enum class EStage : uint8;

USTRUCT(Blueprintable)
struct FStageProperties
{
	GENERATED_BODY()
	
	FStageProperties() :
		StageNumber(0){}

	int32 StageNumber;
};

USTRUCT(Blueprintable)
struct FStageInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FName RowName;
	
	UPROPERTY(BlueprintReadOnly)
	FStageProperties StageProperties;
	
	UPROPERTY(BlueprintReadOnly)
	FStage StageData;
};

DECLARE_DELEGATE_OneParam(FOnLevelLoadSuccess, const FString&);
UCLASS()
class PROJECTT_API UStageManager : public UObject
{
	GENERATED_BODY()

	using StageData = TPair<FName, FStageInfo>;
	
public:
	UStageManager();
	void ReleaseManager();
	void InitializeManager(const UObject* WorldObjectContext, const FString& LevelName);
	void NextLevelLoad();
	int32 GetMaxStage() const;

	UFUNCTION(BlueprintCallable)
	void PrepareLevelLoad(EStage StageType, float FadeDuration = 1.0f, float FadeDelay = 1.0f);

	UFUNCTION(BlueprintCallable)
	void ForceLoadLevel(EStage StageType);

	UFUNCTION(BlueprintCallable)
	FORCEINLINE FStageInfo GetCurrentStageInfo() { return CurrentStageInfo; }

private:
	void PackageLevelLoaded(const FName& PackageName, UPackage* LoadedPackage, EAsyncLoadingResult::Type Result);
	void GetStagePath(EStage StageType, FString& NextStagePath);
	void GetStageData(const FString& LevelName, StageData& OutData) const;
	FName GetLevelDataRowName(FStageProperties StageProperties);
	
private:
	UPROPERTY()
	TObjectPtr<UObject> OuterObject;
	int32 MaxStageNumber;
	FStageInfo CurrentStageInfo;
	FString SaveLevelPath;
	FString LobbyPath;
	FString SelectMapPath;
	FString IntroPath;
};
