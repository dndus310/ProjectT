
// Fill out your copyright notice in the Description page of Project Settings.


#include "CoreGameInstance.h"

#include "HAL/PlatformTime.h"
#include "TimerManager.h"
#include "Blueprint/UserWidget.h"
#include "Common/AssetUtilsLibrary.h"
#include "Common/GlobalUtilsLibrary.h"
#include "ProjectT/System/Core/Managers/NotificationCenter.h"
#include "ProjectT/System/Core/Managers/OptionManager.h"
#include "ProjectT/System/Core/Managers/SoundManager.h"
#include "ProjectT/System/Core/Managers/StageManager.h"
#include "ProjectT/System/Core/Managers/CoreCheatManager.h"
#include "ProjectT/System/Core/Managers/DataDam.h"
#include "Widgets/Images/SThrobber.h"

static double GLoadingStartTime = 0.0;

UCoreGameInstance::UCoreGameInstance() :
	DataDam(nullptr),
	StageManager(nullptr),
	OptionManager(nullptr),
	SoundManager(nullptr),
	CoreCheatManager(nullptr),
    NotificationCenter(nullptr)
{
}

void UCoreGameInstance::Init()
{
	Super::Init();
	
	Execute_GetDataDam(this)->Initialize();
	
	StageManager = NewObject<UStageManager>(this);
	OptionManager = NewObject<UOptionManager>(this);
	SoundManager = NewObject<USoundManager>(this);
	CoreCheatManager = NewObject<UCoreCheatManager>(this);
}

void UCoreGameInstance::BeginDestroy()
{
	Super::BeginDestroy();
	
	DataDam = nullptr;
	StageManager = nullptr;
	OptionManager = nullptr;
	SoundManager = nullptr;
	CoreCheatManager = nullptr;
}

UDataDam* UCoreGameInstance::GetDataDam_Implementation()
{
	if (!DataDam)
	{
		DataDam = NewObject<UDataDam>(this);
	}
	return DataDam;
}

UStageManager* UCoreGameInstance::GetStageManager_Implementation()
{
	if(!StageManager)
	{
		StageManager = NewObject<UStageManager>(this);
	}
	return StageManager;
}

UOptionManager* UCoreGameInstance::GetOptionManager_Implementation()
{
	if (!OptionManager->IsValidLowLevel())
	{
		OptionManager = NewObject<UOptionManager>(this);
	}
	return OptionManager;
}

USoundManager* UCoreGameInstance::GetSoundManager_Implementation()
{
	if (!SoundManager->IsValidLowLevel())
	{
		SoundManager = NewObject<USoundManager>(this);
	}
	return SoundManager;
}

UCoreCheatManager* UCoreGameInstance::GetCoreCheatManager_Implementation()
{
	if(!CoreCheatManager->IsValidLowLevel())
	{
		CoreCheatManager = NewObject<UCoreCheatManager>(this);
	}
	return CoreCheatManager;
}

UNotificationCenter* UCoreGameInstance::GetNotificationCenter_Implementation()
{
	if(!NotificationCenter)
	{
		NotificationCenter = NewObject<UNotificationCenter>(this);
	}
	return NotificationCenter;
}

bool UCoreGameInstance::IsRunningInDevelopment()
{
	EBuildTargetType BT = FApp::GetBuildTargetType();
	EBuildConfiguration BC = FApp::GetBuildConfiguration();
	
	if(BT == EBuildTargetType::Editor) return true; 
	else if(BT == EBuildTargetType::Game && BC == EBuildConfiguration::DebugGame) return true;
	else if(BT == EBuildTargetType::Game && BC == EBuildConfiguration::Development) return true;
	else return false;
}
