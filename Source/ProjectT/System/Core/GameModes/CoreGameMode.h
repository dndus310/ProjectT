// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "ProjectT/System/Core/Interfaces/CoreObjectProvider.h"
#include "CoreGameMode.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTT_API ACoreGameMode : public AGameMode, public ICoreObjectProvider
{
	GENERATED_BODY()

public:
	ACoreGameMode();
	virtual UCoreWidgetManager* GetCoreWidgetManager_Implementation() override;
	
	
protected:
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void InitializeManagers();
	virtual void InitializeUI();

protected:
	UPROPERTY()
	TObjectPtr<class UCoreWidgetManager> CoreWidgetManager;
};
