// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectT/System/Core/Managers/CoreWidgetManager.h"
#include "ProjectT/Data/Gen/GenerateStructStage.h"
#include "ProjectT/Data/Gen/GenerateStructPlayerCharacter.h"
#include "ProjectT/Data/Gen/GenerateStructSkill.h"
#include "ProjectT/Data/Gen/GenerateStructObjective.h"

#include "LobbyWidgetManager.generated.h"

USTRUCT(BlueprintType)
struct FStageWrap
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	TMap<FName, FStage> StageDatas;
};

USTRUCT(BlueprintType)
struct FCharacterWrap
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	TMap<FName, FPlayerCharacter> PlayerCharacterDatas;
};

USTRUCT(BlueprintType)
struct FSkillWrap
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadOnly)
	TMap<FName, FSkill> SkillDatas;
};

USTRUCT(BlueprintType)
struct FObjectiveWrap
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	TMap<FName, FObjective> ObjectiveDatas;
};


DECLARE_DELEGATE_OneParam(FOnRequestLevelChange,int32);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnCompleteLevelChange, int32, StageIDX);

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnRequestStageData, FStageWrap, OutDatas);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnRequestPlayerCharacterData, FCharacterWrap, OutDatas);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnRequestSkillData, FSkillWrap, OutDatas);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnRequestObjectiveData, FObjectiveWrap, OutDatas);


/**
 * 
 */
UCLASS()
class PROJECTT_API ULobbyWidgetManager : public UCoreWidgetManager
{
	GENERATED_BODY()

public:
	virtual void InitializeManager(const UObject* WorldObjectContext) override;	
	virtual void ReleaseManager() override;

	UFUNCTION(BlueprintCallable, Category = "Level")
	void RequestLevelChange(int32 StageIDX, const FOnCompleteLevelChange& Subscribe);

	UFUNCTION(BlueprintCallable, Category = "Data")
	void RequestStageData(const FOnRequestStageData& OnRequestStageData);

	UFUNCTION(BlueprintCallable, Category = "Data")
	void RequestPlayerCharacterData(const FOnRequestPlayerCharacterData& OnRequestPlayerCharacterData);

	UFUNCTION(BlueprintCallable, Category = "Data")
	void RequestSkillData(const FOnRequestSkillData& OnRequestSkillData);

	UFUNCTION(BlueprintCallable, Category = "Data")
	void RequestObjectiveData(const FOnRequestObjectiveData& OnRequestObjectiveData);


public:	
	UPROPERTY()
	TMap<FName, FStage> StageDatas;

	UPROPERTY()
	TMap<FName, FPlayerCharacter> PlayerCharacterDatas;

	UPROPERTY()
	TMap<FName, FSkill> SkillDatas;

	UPROPERTY()
	TMap<FName, FObjective> ObjectiveDatas;	


	UPROPERTY()
	FOnCompleteLevelChange OnCompleteLevelChange;


	FOnRequestLevelChange OnRequestLevelChange;
	

private:
	
};
