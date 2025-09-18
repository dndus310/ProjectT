// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "ProjectT/Data/Gen/GenerateEnumStage.h"
#include "Interfaces/GlobalObjectProvider.h"
#include "Managers/StageManager.h"
#include "CoreGameInstance.generated.h"


class UNotificationCenter;
class USoundManager;
class UOptionManager;
class UCoreCheatManager;
class UQuestManager;

DECLARE_DELEGATE_OneParam(FOnLevelDataChanged, EStage);
UCLASS()
class PROJECTT_API UCoreGameInstance : public UGameInstance, public IGlobalObjectProvider
{
	GENERATED_BODY()

public:
	UCoreGameInstance();

	virtual void Init() override;
	virtual void BeginDestroy() override;

	virtual UDataDam* GetDataDam_Implementation() override;
	virtual UStageManager* GetStageManager_Implementation() override;
	virtual UOptionManager* GetOptionManager_Implementation() override;
	virtual USoundManager* GetSoundManager_Implementation() override;
	virtual UCoreCheatManager* GetCoreCheatManager_Implementation() override;
	virtual UNotificationCenter* GetNotificationCenter_Implementation() override;
	
public:
	UFUNCTION(BlueprintCallable, Category = "IsRunningInDevelopment")
	bool IsRunningInDevelopment();

private:
	UPROPERTY()
	TObjectPtr<UDataDam> DataDam;

	UPROPERTY()
	TObjectPtr<UStageManager> StageManager;

	UPROPERTY()
	TObjectPtr<UOptionManager> OptionManager;

	UPROPERTY()
	TObjectPtr<USoundManager> SoundManager;

	UPROPERTY()
	TObjectPtr<UCoreCheatManager> CoreCheatManager;

	UPROPERTY()
	TObjectPtr<UNotificationCenter> NotificationCenter;
};
