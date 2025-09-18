// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CoreGameMode.h"
#include "ProjectT/System/Core/GameModes/CoreGameMode.h"
#include "ProjectT/System/Core/Interfaces/WorldObjectProvider.h"
#include "WorldGameMode.generated.h"

enum class EProcessState : uint8;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnChangeProcess, EProcessState, ProcessState);

UCLASS()
class PROJECTT_API AWorldGameMode : public ACoreGameMode, public IWorldObjectProvider
{
	GENERATED_BODY()

public:
	AWorldGameMode();
	virtual void Tick(float DeltaSeconds) override;
	virtual USequenceManager* GetSequenceManager_Implementation() override;
	virtual UPooledActorManager* GetPooledActorManager_Implementation() override;
	virtual UQuestManager* GetQuestManager_Implementation() override;
	virtual UCoreWidgetManager* GetCoreWidgetManager_Implementation() override;
	virtual bool ChangedProcessEvent(const EProcessState ProcessState);
	void GetPlayerSpawnPoints(TArray<AActor*>& OutSpawnPoints) const;
	class ACorePlayerCharacter* GetPlayerCharacter();
	void GetNpcCharacters(TArray<AActor*>& OutNpcCharacters) const { OutNpcCharacters = NpcCharacters; }
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void PauseGame();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void UnPauseGame();

protected:
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void InitializeManagers() override;
	virtual void InitializeNotification();
	virtual void InitializeEnemyCharacters();
	virtual void InitializePlayer();
	virtual void InitializeUI() override;

	void InitializeItemObject();
	void InitializePlayerSpawnPoints();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USequenceManager> SequenceManager;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UPooledActorManager> PooledActorManager;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UQuestManager> QuestManager;	

	UPROPERTY()
	TArray<AActor*> PlayerSpawnPoints;

	UPROPERTY()
	TArray<AActor*> NpcCharacters;
};
