// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "LobbyObjectProvider.generated.h"


UINTERFACE(MinimalAPI, Blueprintable)
class ULobbyObjectProvider : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PROJECTT_API ILobbyObjectProvider
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, meta = (Category = "LobbyObjectProvider"))
	class ULobbyWidgetManager* GetLobbyWidgetManager();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, meta = (Category = "LobbyObjectProvider"))
	class ALobbyPreviewCharacter* GetLobbyPreviewCharacter();
	
};
