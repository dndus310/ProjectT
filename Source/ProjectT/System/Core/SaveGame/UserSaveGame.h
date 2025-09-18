// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "ProjectT/Data/User/UserDataModel.h"
#include "UserSaveGame.generated.h"

UCLASS()
class PROJECTT_API UUserSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UUserSaveGame();

	UFUNCTION(BlueprintCallable, Category = "UserSaveGame")
	static void GetUnlockCharacterData(TArray<FUnlockCharacterData>& OutData);

	UFUNCTION(BlueprintCallable, Category = "UserSaveGame")
	static void GetSelectedCharacterData(FString& OutData);

	UFUNCTION(BlueprintCallable, Category = "UserSaveGame")
	static void LoadUserSaveGame(UUserSaveGame*& OutSaveInstance);

	UFUNCTION(BlueprintCallable, Category = "UserSaveGame")
	static void SetCurrentSelectedCharacterData(const FString& InData);

	UFUNCTION(BlueprintCallable, Category = "UserSaveGame")
	static void EmplaceUnlockCharacterData(const FString& RowName, const bool bCleared);

	UFUNCTION(BlueprintCallable, Category = "UserSaveGame")
	static void GetUnlockStageData(TArray<FStageData>& OutData);

	UFUNCTION(BlueprintCallable, Category = "UserSaveGame")
	static void EmplaceUnlockStageData(const FString& RowName, const bool bCleared);

private:
	static void GameSave(UUserSaveGame* InSaveInstance);

	UFUNCTION()
	static UUserSaveGame* GetSaveInstance();
	
private:
	UPROPERTY()
	FUserSaveData UserSaveData;

	static const FString SaveSlotName;
};
