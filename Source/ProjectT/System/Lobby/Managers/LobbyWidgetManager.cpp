// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyWidgetManager.h"
#include "ProjectT/System/Lobby/GameModes/LobbyGameMode.h"

void ULobbyWidgetManager::InitializeManager(const UObject* WorldObjectContext)
{
	Super::InitializeManager(WorldObjectContext);
}

void ULobbyWidgetManager::ReleaseManager()
{	
	OnRequestLevelChange.Unbind();

	Super::ReleaseManager();
}

void ULobbyWidgetManager::RequestLevelChange(int32 StageIDX, const FOnCompleteLevelChange& Subscribe)
{
	OnRequestLevelChange.ExecuteIfBound(StageIDX);
	OnCompleteLevelChange = Subscribe;	
}

void ULobbyWidgetManager::RequestStageData(const FOnRequestStageData& OnRequestStageData)
{
	FStageWrap Data;
	Data.StageDatas = StageDatas;
	OnRequestStageData.ExecuteIfBound(Data);
}

void ULobbyWidgetManager::RequestPlayerCharacterData(const FOnRequestPlayerCharacterData& OnRequestPlayerCharacterData)
{
	FCharacterWrap Data;
	Data.PlayerCharacterDatas = PlayerCharacterDatas;
	OnRequestPlayerCharacterData.ExecuteIfBound(Data);
}

void ULobbyWidgetManager::RequestSkillData(const FOnRequestSkillData& OnRequestSkillData)
{
	FSkillWrap Data;
	Data.SkillDatas = SkillDatas;
	OnRequestSkillData.ExecuteIfBound(Data);
}

void ULobbyWidgetManager::RequestObjectiveData(const FOnRequestObjectiveData& OnRequestObjectiveData)
{
	FObjectiveWrap Data;
	Data.ObjectiveDatas = ObjectiveDatas;
	OnRequestObjectiveData.ExecuteIfBound(Data);
}

