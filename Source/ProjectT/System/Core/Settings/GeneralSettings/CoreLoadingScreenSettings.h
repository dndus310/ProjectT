// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "CoreLoadingScreenSettings.generated.h"

/**
 * 
 */
UCLASS(config=Game, defaultconfig, meta=(DisplayName="LoadingScreenSettings"))
class PROJECTT_API UCoreLoadingScreenSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UCoreLoadingScreenSettings();

	/**
	 * The movie paths local to the game's Content/Movies/ directory we will play.
	 */
	UPROPERTY(config, EditAnywhere, Category = Display)
	bool bEnableStartUpMovie;

	UPROPERTY(config, EditAnywhere, Category = Display, meta = (EditCondition = "bEnableStartUpMovie"))
	FString StartUpMoviePath;

	UPROPERTY(config, EditAnywhere, Category = Display, meta = (MetaClass = "/Script/UMG.UserWidget", EditCondition = "!bEnableStartUpMovie"))
	FSoftClassPath PreLoadingScreenClassPath;

	UPROPERTY(config, EditAnywhere, Category = Display, meta = (MetaClass = "/Script/UMG.UserWidget"))
	FSoftClassPath PostLoadingScreenClassPath;

	UPROPERTY(config, EditAnywhere, Category = Display)
	float ZOrder;

	UPROPERTY(config, EditAnywhere, Category = Display, meta= (ClampMin = "2.0", UIMin = "2.0"))
	float MinimumLoadingScreenTime;

	UPROPERTY(config, EditAnywhere, Category = Display)
	uint8 bForceLoadingComplete : 1;
};
