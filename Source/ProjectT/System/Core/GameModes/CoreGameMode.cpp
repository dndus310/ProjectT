// Fill out your copyright notice in the Description page of Project Settings.


#include "CoreGameMode.h"

#include "ProjectT/ProjectT.h"
#include "ProjectT/System/Core/Common/GlobalUtilsLibrary.h"
#include "ProjectT/System/Core/Interfaces/GlobalObjectProvider.h"
#include "ProjectT/System/Core/Managers/CoreCheatManager.h"
#include "ProjectT/System/Core/Managers/LoadingManager.h"
#include "ProjectT/System/Core/Managers/NotificationCenter.h"
#include "ProjectT/System/Core/Managers/SoundManager.h"
#include "ProjectT/System/Core/Managers/StageManager.h"

ACoreGameMode::ACoreGameMode() :
	CoreWidgetManager(nullptr)
{
}

UCoreWidgetManager* ACoreGameMode::GetCoreWidgetManager_Implementation()
{
	return CoreWidgetManager;
}

void ACoreGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	UWorld* World = UGlobalUtilsLibrary::GetValidWorld(this);

	UGameInstance* GI = GetGameInstance();
	NMT_CHECKF(GI);
	
	UStageManager* StageMgr = IGlobalObjectProvider::Execute_GetStageManager(GI);
	NMT_CHECKF(StageMgr);

	const FString& CurrentLevelName = UGameplayStatics::GetCurrentLevelName(World);
	StageMgr->InitializeManager(World, CurrentLevelName);
	
	USoundManager* SoundMgr = IGlobalObjectProvider::Execute_GetSoundManager(GI);
	NMT_CHECKF(SoundMgr);
	SoundMgr->InitializeManager(World);

	UCoreCheatManager* CheatMgr = IGlobalObjectProvider::Execute_GetCoreCheatManager(GI);
	NMT_CHECKF(CheatMgr);
	CheatMgr->InitializeManager(World);
}

void ACoreGameMode::BeginPlay()
{
	Super::BeginPlay();
}

void ACoreGameMode::BeginDestroy()
{
	Super::BeginDestroy();
}

void ACoreGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	UGameInstance* GI = GetGameInstance();
	NMT_CHECKF(GI);

	UNotificationCenter* NC = IGlobalObjectProvider::Execute_GetNotificationCenter(GI);
	NC->ReleaseNotificationCenter();
	
	USoundManager* SoundMgr = IGlobalObjectProvider::Execute_GetSoundManager(GI);
	SoundMgr->ReleaseManager();
	
	UStageManager* StageMgr = IGlobalObjectProvider::Execute_GetStageManager(GI);
	StageMgr->ReleaseManager();

	UCoreCheatManager* CheatMgr = IGlobalObjectProvider::Execute_GetCoreCheatManager(GI);
	NMT_CHECKF(CheatMgr);
	CheatMgr->ReleaseManager();
	
}

void ACoreGameMode::InitializeManagers()
{
}

void ACoreGameMode::InitializeUI()
{
}
