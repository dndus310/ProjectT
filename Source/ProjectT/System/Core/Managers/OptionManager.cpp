// Fill out your copyright notice in the Description page of Project Settings.

#include "OptionManager.h"
#include "StageManager.h"
#include "ProjectT/System/Core/Common/GlobalUtilsLibrary.h"
#include "ProjectT/System/Core/Managers/NotificationCenter.h"
#include "ProjectT/System/Core/Interfaces/GlobalObjectProvider.h"

UOptionManager::UOptionManager()
{
}

void UOptionManager::InitializeManager(const UObject* WorldObjectContext)
{
	OuterObject = const_cast<UObject*>(WorldObjectContext);	
}

// void UOptionManager::InitializeOptionData()
// {
// 	GameOptionData = UPlayerGameSaveData::LoadOptionData();
// }
//
// FGamePlayOption UOptionManager::GetGamePlayOption() const
// {
// 	return GameOptionData.GamePlayOption;
// }
//
// FAudioOption UOptionManager::GetAudioOption() const
// {
// 	return GameOptionData.AudioOption;
// }
//
// void UOptionManager::SetGamePlayOption(FGamePlayOption OptionData)
// {
// 	bool LanguageChanged = (GameOptionData.GamePlayOption.Language != OptionData.Language);
//
// 	GameOptionData.GamePlayOption = OptionData;
// 	UPlayerGameSaveData::SaveOptionData(GameOptionData);
// 	
// 	UNotificationCenter* NC = UGlobalUtilsLibrary::GetSharedNotificationCenter(this);
// 	if(LanguageChanged)
// 	{
// 		NC->Post(ENotificationTypes::Option_Language_Changed);
// 	}
// }
//
// void UOptionManager::SetAudioOption(FAudioOption OptionData)
// {
// 	GameOptionData.AudioOption = OptionData;
// 	UPlayerGameSaveData::SaveOptionData(GameOptionData);
// }
//
// void UOptionManager::ResetGameOptionData()
// {	
// 	UWorld* World = UGlobalUtilsLibrary::GetValidWorld(OuterObject);
// 	ULevelManager* LM = IGlobalObjectProvider::Execute_GetLevelManager(World->GetGameInstance());
//
// 	bool LanguageChanged = (GameOptionData.GamePlayOption.Language != UPlayerGameSaveData::GetGameLanguageFromDevice());
// 	
// 	GameOptionData = UPlayerGameSaveData::LoadOptionData();
// 	
// 	UNotificationCenter* NC = UGlobalUtilsLibrary::GetSharedNotificationCenter(this);
// 	if(LanguageChanged)
// 	{
// 		NC->Post(ENotificationTypes::Option_Language_Changed);
// 	}
// }

void UOptionManager::ReleaseManager()
{
	OuterObject = nullptr;
}