// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "Sound/SoundMix.h"
#include "CoreGameGeneralSettings.generated.h"

/**
 * NOTE: ProjectT에 사용되는 보편적인 시스템 및 에셋 클래스를 정의합니다. 
 */

class USoundClass;
class USoundMix;
class ASpectatorPawn;

UCLASS(Config=Game, DefaultConfig, meta = (DisplayName = "General Class Settings"))
class PROJECTT_API UCoreGameGeneralSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	static UCoreGameGeneralSettings* Get();

	UFUNCTION(BlueprintCallable,Category="GeneralSound")
	static TSoftObjectPtr<USoundClass> GetMasterSoundClass()
	{
		return UCoreGameGeneralSettings::Get()->Sound_Master;
	}

	UFUNCTION(BlueprintCallable,Category="GeneralSound")
	static TSoftObjectPtr<USoundClass> GetSFXSoundClass()
	{
		return UCoreGameGeneralSettings::Get()->Sound_BGM;
	}

	UFUNCTION(BlueprintCallable,Category="GeneralSound")
	static TSoftObjectPtr<USoundClass> GetBGMSoundClass()
	{
		return UCoreGameGeneralSettings::Get()->Sound_SFX;
	}

	UFUNCTION(BlueprintCallable,Category="GeneralSound")
	static TSoftObjectPtr<USoundMix> GetSoundMixClass()
	{
		return UCoreGameGeneralSettings::Get()->SoundMixClass_Master;
	}

	UFUNCTION(BlueprintCallable, Category = "DebugSpectator")
	static TSubclassOf<ASpectatorPawn> GetDebugSpectatorPawn()
	{
		return UCoreGameGeneralSettings::Get()->DebugSpectatorPawn;
	}

private:
	UPROPERTY(Config,EditAnywhere,BlueprintReadOnly,Category="GeneralSound", meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<USoundClass> Sound_Master;

	UPROPERTY(Config,EditAnywhere,BlueprintReadOnly,Category="GeneralSound", meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<USoundClass> Sound_BGM;

	UPROPERTY(Config,EditAnywhere,BlueprintReadOnly,Category="GeneralSound", meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<USoundClass> Sound_SFX;

	UPROPERTY(Config,EditAnywhere,BlueprintReadOnly,Category="GeneralSound", meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<USoundMix> SoundMixClass_Master;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "SinglePlay", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<ASpectatorPawn> DebugSpectatorPawn;
};
