// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectT/System/Core/GameModes/CoreGameMode.h"
#include "ProjectT/System/Lobby/Interfaces/LobbyObjectProvider.h"
#include "LobbyGameMode.generated.h"

/**
 * 
 */

class ULobbyWidgetManager;

UCLASS()
class PROJECTT_API ALobbyGameMode : public ACoreGameMode, public ILobbyObjectProvider
{
	GENERATED_BODY()
	
public:
	ALobbyGameMode();
	virtual void Tick(float DeltaSeconds) override;
	virtual ULobbyWidgetManager* GetLobbyWidgetManager_Implementation() override;
	virtual ALobbyPreviewCharacter* GetLobbyPreviewCharacter_Implementation() override;



protected:
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void InitializeManagers() override;
	virtual void InitializeUI() override;
	
private:
	UFUNCTION()
	void LevelChange(int32 LobbyLevelIDX);

	UFUNCTION()
	void CompleteLevelLoad();

	UFUNCTION()
	void CompleteLevelUnLoad();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LobbyLevels")
	TArray<TSoftObjectPtr<UWorld>> LobbyLevels;	

private:
	UPROPERTY()
	TSoftObjectPtr<UWorld> CurrentStreamLevel;

	UPROPERTY()
	int32 StageIDX;
	
};
