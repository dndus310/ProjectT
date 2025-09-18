// Fill out your copyright notice in the Description page of Project Settings.


#include "UserSaveGame.h"

#include "Kismet/GameplayStatics.h"
#include "ProjectT/ProjectT.h"


const FString UUserSaveGame::SaveSlotName = TEXT("UserSaveData");
UUserSaveGame::UUserSaveGame()
{
}

void UUserSaveGame::LoadUserSaveGame(UUserSaveGame*& OutSaveInstance)
{
	USaveGame* SaveGame = UGameplayStatics::LoadGameFromSlot(SaveSlotName, 0);
	if(!SaveGame)
	{
		SaveGame = UGameplayStatics::CreateSaveGameObject(UUserSaveGame::StaticClass());
		UGameplayStatics::SaveGameToSlot(SaveGame, SaveSlotName, 0);
	}
	OutSaveInstance = CastChecked<UUserSaveGame>(SaveGame);
}

void UUserSaveGame::SetCurrentSelectedCharacterData(const FString& InData)
{
	UUserSaveGame* SaveInstance = GetSaveInstance();
	SaveInstance->UserSaveData.SelectedCharacterData = InData;

	GameSave(SaveInstance);
}

void UUserSaveGame::EmplaceUnlockCharacterData(const FString& RowName, const bool bCleared)
{
	UUserSaveGame* SaveInstance = GetSaveInstance();
	int32 FoundIndex = SaveInstance->UserSaveData.UnlockCharacterData.IndexOfByPredicate([RowName](const FUnlockCharacterData& CharacterData)
	{
		return CharacterData.UnlockCharacterRowName == RowName;
	});
	FUnlockCharacterData Data;
	if(FoundIndex == INDEX_NONE) Data = FUnlockCharacterData(RowName, bCleared);
	else SaveInstance->UserSaveData.UnlockCharacterData[FoundIndex].bCleared = bCleared;
		
	SaveInstance->UserSaveData.UnlockCharacterData.Push(Data);
	GameSave(SaveInstance);
}

void UUserSaveGame::GetUnlockStageData(TArray<FStageData>& OutData)
{
	OutData = GetSaveInstance()->UserSaveData.UnlockStageData;
}

void UUserSaveGame::EmplaceUnlockStageData(const FString& RowName, const bool bCleared)
{
	UUserSaveGame* SaveInstance = GetSaveInstance();
	int32 FoundIndex = SaveInstance->UserSaveData.UnlockStageData.IndexOfByPredicate([RowName](const FStageData& StageData)
		{
			return StageData.StageRowName == RowName;
		});
	FStageData Data;
	if(FoundIndex == INDEX_NONE) Data = FStageData(RowName, bCleared);
	else SaveInstance->UserSaveData.UnlockCharacterData[FoundIndex].bCleared = bCleared;

	SaveInstance->UserSaveData.UnlockStageData.Push(Data);
	GameSave(SaveInstance);
}

void UUserSaveGame::GameSave(UUserSaveGame* InSaveInstance)
{
	NMT_CHECKF(InSaveInstance);
	UGameplayStatics::SaveGameToSlot(InSaveInstance, SaveSlotName, 0);
}

UUserSaveGame* UUserSaveGame::GetSaveInstance()
{
	UUserSaveGame* Ret = nullptr;
	LoadUserSaveGame(Ret);
	
	return Ret;
}

void UUserSaveGame::GetUnlockCharacterData(TArray<FUnlockCharacterData>& OutData)
{
	OutData = GetSaveInstance()->UserSaveData.UnlockCharacterData;
}

void UUserSaveGame::GetSelectedCharacterData(FString& OutData)
{
	OutData = GetSaveInstance()->UserSaveData.SelectedCharacterData;
}
