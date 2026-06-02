// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectT/System/Core/Characters/Player/CorePlayerCharacter.h"
#include "ProjectT/System/Core/GameModes/WorldGameMode.h"
#include "ProjectT/System/Core/Interfaces/LoadableObject.h"
#include "ProjectT/System/SinglePlay/Interfaces/SPObjectProvider.h"
#include "SPGameModeBase.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUpdateRemainingTime, int32, RemainSeconds);

UCLASS()
class PROJECTT_API ASPGameModeBase : public AWorldGameMode, public ISPObjectProvider, public ILoadableObject
{
	GENERATED_BODY()

public:
	ASPGameModeBase();
	virtual void Tick(float DeltaSeconds) override;
	virtual bool ChangedProcessEvent(const EProcessState ProcessState) override;
	virtual USPWidgetManager* GetSPWidgetManager_Implementation() override;
	virtual USPEnemyVisionManager* GetEnemyVisionManager_Implementation() override;
	virtual UQuestManager* GetQuestManager_Implementation() override;
	virtual bool HasFinishedLoading_Implementation() override;

	UFUNCTION(BlueprintNativeEvent)
	void InitializeMapData();

	void DebugGameTimePause(bool bPause);
	
protected:
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void InitializeManagers() override;
	virtual void InitializeNotification() override;
	virtual void InitializeEnemyCharacters() override;
	virtual void InitializePlayer() override;
	virtual void InitializeUI() override;
	
private:
	void InitializeExitPoint();
	void SetStageTimeLimit();
	void InitializeEnemyByFlag();
	
public:
	UPROPERTY(BlueprintAssignable)
	FOnUpdateRemainingTime OnUpdateRemainingTime;

private:
	UPROPERTY()
	TObjectPtr<class USPEnemyVisionManager> SPEnemyVisionManager;
	
	UPROPERTY()
	FTimerHandle StageRemainTimeHandle;

	UPROPERTY()
	int32 RemainingSeconds;

	UPROPERTY()
	TObjectPtr<class AExitPoint> ExitPoint;
};
