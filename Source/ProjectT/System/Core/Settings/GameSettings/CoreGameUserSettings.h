// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameUserSettings.h"
#include "Kismet/GameplayStatics.h"
#include "ProjectT/ProjectT.h"
#include "CoreGameUserSettings.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class EGameLanguage : uint8
{
	Korea = 0 UMETA(DisplayName = "Korea"),
	English UMETA(DisplayName = "English"),
	ChineseCN UMETA(DisplayName = "ChineseCN"),
	ChineseTW UMETA(DisplayName = "ChineseTW"),
	Spanish UMETA(DisplayName = "Spanish"),
};

UENUM(BlueprintType)
enum class EGameOptionType : uint8
{
	Language = 0 UMETA(DisplayName = "Language"),
	Resolution UMETA(DisplayName = "Resolution"),
	WindowMode UMETA(DisplayName = "WindowMode"),
	Gamma UMETA(DisplayName = "Gamma"),
	Scalability UMETA(DisplayName = "Scalability"),
	AntiAliasingQuality UMETA(DisplayName = "AntiAliasing Quality"),
	ShadowQuality UMETA(DisplayName = "Shadow Quality"),
	MasterVolume UMETA(DisplayName = "MasterVolume"),
	BGMVolume UMETA(DisplayName = "BGM Volume"),
	SFXVolume UMETA(DisplayName = "SFX Volume"),
	// Display,
	// VSync,
	// TextureQuality,
	// EffectQuality,
};

UENUM(BlueprintType)
enum class EResolution : uint8
{
	None = 0 UMETA(DisplayName = "None"),
	Res_3840_2160 UMETA(DisplayName = "Res_3840_2160"),
	Res_2560_1600 UMETA(DisplayName = "Res_2560_1600"),
	Res_2560_1440 UMETA(DisplayName = "Res_2560_1440"),
	Res_1920_1440 UMETA(DisplayName = "Res_1920_1440"),
	Res_1920_1200 UMETA(DisplayName = "Res_1920_1200"),
	Res_1920_1080 UMETA(DisplayName = "Res_1920_1080"),
	Res_1600_1200 UMETA(DisplayName = "Res_1600_1200"),
	Res_1600_1024 UMETA(DisplayName = "Res_1600_1024"),
	Res_1600_900 UMETA(DisplayName = "Res_1600_900"),
	Res_1280_960 UMETA(DisplayName = "Res_1280_960"),
	Res_1280_800 UMETA(DisplayName = "Res_1280_800"),
	Res_1280_720 UMETA(DisplayName = "Res_1280_720"),
};

USTRUCT(Blueprintable)
struct FGamePlayOption
{
	GENERATED_BODY()

	FGamePlayOption() :
		System_Language(EGameLanguage::Korea),
		Setting_Language(EGameLanguage::Korea),
		Setting_Resolution(EResolution::Res_1920_1080),
		Setting_WindowMode(EWindowMode::Type::Fullscreen),
		Setting_Gamma(1.f)
	{}
	
	/*Language*/
	UPROPERTY(BlueprintReadOnly, Category = "GamePlayOption")
	EGameLanguage System_Language;
	
	UPROPERTY(Config, BlueprintReadOnly, Category = "GamePlayOption")
	EGameLanguage Setting_Language;

	/*Display*/
	UPROPERTY(Config, BlueprintReadOnly, Category = "GamePlayOption")
	EResolution Setting_Resolution;

	UPROPERTY(Config, BlueprintReadOnly, Category = "GamePlayOption")
	TEnumAsByte<EWindowMode::Type> Setting_WindowMode;
	
	UPROPERTY(Config, BlueprintReadOnly, Category = "GamePlayOption")
	float Setting_Gamma;
};

USTRUCT(Blueprintable)
struct FGraphicOption
{
	GENERATED_BODY()

	FGraphicOption() :
		Setting_ScalabilityQuality(Scalability::DefaultQualityLevel),
		Setting_AntialiasingQuality(Scalability::DefaultQualityLevel),
		Setting_ShadowQuality(Scalability::DefaultQualityLevel)
	{}
	
	UPROPERTY(Config, BlueprintReadOnly, Category = "GraphicOption")
	int32 Setting_ScalabilityQuality;

	UPROPERTY(Config, BlueprintReadOnly, Category = "GraphicOption")
	int32 Setting_AntialiasingQuality;

	UPROPERTY(Config, BlueprintReadOnly, Category = "GraphicOption")
	int32 Setting_ShadowQuality;
};

USTRUCT(Blueprintable)
struct FAudioOption
{
	GENERATED_BODY()

	FAudioOption() :
		Setting_MasterVolume(1.f),
		Setting_BGMVolume(1.f),
		Setting_SFXVolume(1.f)
	{}
	
	UPROPERTY(Config, BlueprintReadOnly, Category = "AudioOption")
	float Setting_MasterVolume;

	UPROPERTY(Config, BlueprintReadOnly, Category = "AudioOption")
	float Setting_BGMVolume;

	UPROPERTY(Config, BlueprintReadOnly, Category = "AudioOption")
	float Setting_SFXVolume;
};

USTRUCT(Blueprintable)
struct FGameOptionData
{
	GENERATED_BODY()

	FGameOptionData() :
		GamePlayOption(FGamePlayOption()),
		GraphicOption(FGraphicOption()),
		AudioOption(FAudioOption())
	{}
	
	UPROPERTY(Config, BlueprintReadOnly, Category = "GamePlayOption")
	FGamePlayOption GamePlayOption;

	UPROPERTY(Config, BlueprintReadOnly, Category = "GraphicOption")
	FGraphicOption GraphicOption;
	
	UPROPERTY(Config, BlueprintReadOnly, Category = "AudioOption")
	FAudioOption AudioOption;
};

UCLASS(config=GameUserSettings)
class PROJECTT_API UCoreGameUserSettings : public UGameUserSettings
{
	GENERATED_BODY()

public:
	UCoreGameUserSettings();

	UFUNCTION(BlueprintPure, Category = "GameUserSetting")
	static UCoreGameUserSettings* GetCoreGameUserSettings();

public:
	void InitializeUserSettings(const bool bReset = false);

	UFUNCTION(BlueprintPure, Category = "GameOptionData")
	FGameOptionData GetGameOptionData() const { return UserGameOptionData; }

	UFUNCTION(BlueprintPure, Category = "GamePlayOption")
	FGamePlayOption GetGamePlayOption() const { return UserGameOptionData.GamePlayOption; }

	UFUNCTION(BlueprintPure, Category = "GraphicOption")
	FGraphicOption GetGraphicOption() const { return UserGameOptionData.GraphicOption; }

	UFUNCTION(BlueprintPure, Category = "AudioOption")
	FAudioOption GetAudioOption() const { return UserGameOptionData.AudioOption; }
	
	UFUNCTION(BlueprintCallable, Category = "GameUserSetting")
	void InitializeToDefault();
	
	UFUNCTION(BlueprintCallable, Category = "GameUserSetting")
	void ChangeSettingByEnum(const EGameOptionType InOptionType, const int32 InOptionLevel, const float InOptionValue);

private:
	void Initialize();
	
	void SetSystemLanguage();
	void SetUserLanguage(const EGameLanguage InLanguage);
	void SetUserScreenResolution(const EResolution InResolution);
	void SetUserWindowMode(const EWindowMode::Type InWindowMode);
	void SetUserGamma(const float InValue);
	void SetUserScalability(const int32 InLevel);
	void SetUserAntialiasingQuality(const int32 InLevel);
	void SetUserShadowQuality(const int32 InLevel);
	
	FORCEINLINE void SetSoundMixClassOverride(USoundClass* InSoundClass, const float InVolume) const
	{
		UWorld* World = GEngine->GameViewport->GetWorld();
		NMT_ENSURE(World);
		
		UGameplayStatics::SetSoundMixClassOverride(World, SoundMix, InSoundClass, InVolume, 1.0f, 0.f, false);
	}
	
	FORCEINLINE void SetUserMasterVolume(const float InVolume) const
	{
		SetSoundMixClassOverride(MasterSoundClass, InVolume);
	}
	
	FORCEINLINE void SetUserBGMVolume(const float InVolume) const
	{
		SetSoundMixClassOverride(BGMSoundClass, InVolume);
	}

	FORCEINLINE void SetUserSFXVolume(const float InVolume) const
	{
		SetSoundMixClassOverride(SFXSoundClass, InVolume);
	}

private:
	UPROPERTY(Config)
	FGameOptionData UserGameOptionData;

	UPROPERTY()
	USoundClass* MasterSoundClass;

	UPROPERTY()
	USoundClass* BGMSoundClass;
	
	UPROPERTY()
	USoundClass* SFXSoundClass;

	UPROPERTY()
	USoundMix* SoundMix;
};
