// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "WidgetInputHandler.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, Blueprintable)
class UWidgetInputHandler : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PROJECTT_API IWidgetInputHandler
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, meta = (Category = "Widget Input"))
	void OnInit(int32 ContainerIndex);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, meta = (Category = "Widget Input"))
	void OnNavigate(const FVector2D& Direction);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, meta = (Category = "Widget Input"))
	void OnHovered();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, meta = (Category = "Widget Input"))
	void OnUnhovered();
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, meta = (Category = "Widget Input"))
	void OnClicked();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, meta = (Category = "Widget Input"))
	void OnPressed();
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, meta = (Category = "Widget Input"))
	void OnReleased();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, meta = (Category = "Widget Input"))
	void OnBack();
};
