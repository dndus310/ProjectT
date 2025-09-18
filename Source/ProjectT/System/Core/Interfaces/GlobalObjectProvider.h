// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GlobalObjectProvider.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UGlobalObjectProvider : public UInterface
{
	GENERATED_BODY()
};

class PROJECTT_API IGlobalObjectProvider
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, meta = (Category = "DataDam"))
	class UDataDam* GetDataDam();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, meta = (Category = "LevelManager"))
	class UStageManager* GetStageManager();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, meta = (Category = "OptionManager"))
	class UOptionManager* GetOptionManager();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, meta = (Category = "SoundManager"))
	class USoundManager* GetSoundManager();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, meta = (Category = "UCheatManager"))
	class UCoreCheatManager* GetCoreCheatManager();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, meta = (Category = "UNotificationCenter"))
	class UNotificationCenter* GetNotificationCenter();
};
