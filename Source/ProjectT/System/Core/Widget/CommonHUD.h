// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CommonHUD.generated.h"

UCLASS()
class PROJECTT_API UCommonHUD : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent)
	void ToggleGameMenuEvent();

protected:
	virtual void NativeConstruct() override;
	virtual void InitializeBindEvent();
};
