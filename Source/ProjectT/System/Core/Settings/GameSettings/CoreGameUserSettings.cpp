// Fill out your copyright notice in the Description page of Project Settings.


#include "CoreGameUserSettings.h"
#include "Sound/SoundClass.h"
#include "Sound/SoundMix.h"
#include "ProjectT/System/Core/Common/GlobalUtilsLibrary.h"
#include "ProjectT/System/Core/Settings/GeneralSettings/CoreGameGeneralSettings.h"
#include "ProjectT/System/Core/Managers/NotificationCenter.h"

UCoreGameUserSettings::UCoreGameUserSettings()
{
	Initialize();
}

UCoreGameUserSettings* UCoreGameUserSettings::GetCoreGameUserSettings()
{
	return Cast<UCoreGameUserSettings>(GetGameUserSettings());
}

void UCoreGameUserSettings::Initialize()
{
	UserGameOptionData = FGameOptionData();
	SetSystemLanguage();
}

void UCoreGameUserSettings::InitializeToDefault()
{
	Initialize();
	InitializeUserSettings(true);
}

void UCoreGameUserSettings::InitializeUserSettings(const bool bReset/* = false*/)
{
	if (NMT_ENSURE(!UCoreGameGeneralSettings::GetMasterSoundClass().IsNull() && !UCoreGameGeneralSettings::GetBGMSoundClass().IsNull() && !UCoreGameGeneralSettings::GetSFXSoundClass().IsNull() && !UCoreGameGeneralSettings::GetSoundMixClass().IsNull()))
	{
		MasterSoundClass = UCoreGameGeneralSettings::GetMasterSoundClass().Get();
		BGMSoundClass = UCoreGameGeneralSettings::GetBGMSoundClass().Get();
		SFXSoundClass = UCoreGameGeneralSettings::GetSFXSoundClass().Get();
		SoundMix = UCoreGameGeneralSettings::GetSoundMixClass().Get();
	}
	
	bReset ? SetUserLanguage(UserGameOptionData.GamePlayOption.System_Language) : SetUserLanguage(UserGameOptionData.GamePlayOption.Setting_Language);
	SetUserScreenResolution(UserGameOptionData.GamePlayOption.Setting_Resolution);
	SetUserWindowMode(UserGameOptionData.GamePlayOption.Setting_WindowMode);
	SetUserGamma(UserGameOptionData.GamePlayOption.Setting_Gamma);
	
	SetUserAntialiasingQuality(UserGameOptionData.GraphicOption.Setting_AntialiasingQuality);
	SetUserShadowQuality(UserGameOptionData.GraphicOption.Setting_ShadowQuality);
	SetUserScalability(UserGameOptionData.GraphicOption.Setting_ScalabilityQuality);

	SetUserMasterVolume(UserGameOptionData.AudioOption.Setting_MasterVolume);
	SetUserBGMVolume(UserGameOptionData.AudioOption.Setting_BGMVolume);
	SetUserSFXVolume(UserGameOptionData.AudioOption.Setting_SFXVolume);

	UGameplayStatics::SetBaseSoundMix(GEngine->GameViewport->GetWorld(), SoundMix);
}

void UCoreGameUserSettings::ChangeSettingByEnum(const EGameOptionType InOptionType, const int32 InOptionLevel, const float InOptionValue)
{
	switch (InOptionType)
	{
	case EGameOptionType::Language:
		SetUserLanguage(static_cast<EGameLanguage>(InOptionLevel));
		break;
	case EGameOptionType::Resolution:
		SetUserScreenResolution(static_cast<EResolution>(InOptionLevel));
		break;
	case EGameOptionType::WindowMode:
		SetUserWindowMode(static_cast<EWindowMode::Type>(InOptionLevel));
		break;
	case EGameOptionType::Gamma:
		SetUserGamma(InOptionValue);
		break;
	case EGameOptionType::Scalability:
		SetUserShadowQuality(InOptionLevel);
		break;
	case EGameOptionType::AntiAliasingQuality:
		SetUserAntialiasingQuality(InOptionLevel);
		break;
	case EGameOptionType::ShadowQuality:
		SetUserShadowQuality(InOptionLevel);
		break;
	case EGameOptionType::MasterVolume:
		SetUserMasterVolume(InOptionValue);
		break;
	case EGameOptionType::BGMVolume:
		SetUserBGMVolume(InOptionValue);
		break;
	case EGameOptionType::SFXVolume:
		SetUserSFXVolume(InOptionValue);
		break;
	default: break;
	}
}

void UCoreGameUserSettings::SetSystemLanguage()
{
	const FString ISOCode = UKismetSystemLibrary::GetDefaultLanguage();
	EGameLanguage Language = EGameLanguage::English;
	
	if (ISOCode.StartsWith(TEXT("ko")))
	{
		Language = EGameLanguage::Korea;
	}
	else if (ISOCode.StartsWith(TEXT("en")))
	{
		Language = EGameLanguage::English;
	}
	else if (ISOCode.StartsWith(TEXT("zh-CN")))
	{
		Language = EGameLanguage::ChineseCN;
	}
	else if (ISOCode.StartsWith(TEXT("zh-TW")))
	{
		Language = EGameLanguage::ChineseTW;
	}
	else if (ISOCode.StartsWith(TEXT("es")))
	{
		Language = EGameLanguage::Spanish;
	}
	
	UserGameOptionData.GamePlayOption.System_Language = Language;
}

void UCoreGameUserSettings::SetUserLanguage(const EGameLanguage InLanguage)
{
	bool bLanguageChanged = UserGameOptionData.GamePlayOption.Setting_Language != InLanguage;
	
	UserGameOptionData.GamePlayOption.Setting_Language = InLanguage;
	
	SaveSettings();
	if (bLanguageChanged)
	{
		UNotificationCenter* NC = UGlobalUtilsLibrary::GetSharedNotificationCenter(this);
		NC->Post(ENotificationTypes::Option_Language_Changed);
	}
}

void UCoreGameUserSettings::SetUserScreenResolution(const EResolution InResolution)
{
	const FString ResolutionStr = UGlobalUtilsLibrary::GetEnumToString<EResolution>(InResolution);
	NMT_ENSURE(!ResolutionStr.IsEmpty());
		
	const TArray<FString> ParsedResult = UGlobalUtilsLibrary::ParseStringIndices(ResolutionStr, "_");
	NMT_ENSURE(ParsedResult.Num() >= 3);
	if(ParsedResult[1].IsEmpty() || ParsedResult[2].IsEmpty())
	{
		NMT_MSG_ENSURE(0, "ParsedResult Array is Empty");
		return;
	}
	
	const int32 Width = FCString::Atoi(*ParsedResult[1]);
	const int32 Height = FCString::Atoi(*ParsedResult[2]);
	
	UserGameOptionData.GamePlayOption.Setting_Resolution = InResolution;
	
	SetScreenResolution(FIntPoint(Width, Height));
	ApplySettings(true);
}

void UCoreGameUserSettings::SetUserWindowMode(const EWindowMode::Type InWindowMode)
{
	if (InWindowMode >= EWindowMode::Type::NumWindowModes)
	{
		NMT_LOG("WindowMode can only be one of three types");
		return;
	}
	
	UserGameOptionData.GamePlayOption.Setting_WindowMode = InWindowMode;
	
	SetFullscreenMode(InWindowMode);
	ApplySettings(true);
}

void UCoreGameUserSettings::SetUserGamma(const float InValue)
{
	UserGameOptionData.GamePlayOption.Setting_Gamma = InValue;

	ApplySettings(false);
}

void UCoreGameUserSettings::SetUserScalability(const int32 InLevel)
{
	UserGameOptionData.GraphicOption.Setting_ScalabilityQuality = InLevel;
	
	SetUserAntialiasingQuality(InLevel);
	SetUserShadowQuality(InLevel);
}

void UCoreGameUserSettings::SetUserAntialiasingQuality(const int32 InLevel)
{
	UserGameOptionData.GraphicOption.Setting_AntialiasingQuality = InLevel;
	
	SetAntiAliasingQuality(InLevel);
	ApplySettings(false);
}

void UCoreGameUserSettings::SetUserShadowQuality(const int32 InLevel)
{
	UserGameOptionData.GraphicOption.Setting_ShadowQuality = InLevel;
	
	SetShadowQuality(InLevel);
	ApplySettings(false);
}


