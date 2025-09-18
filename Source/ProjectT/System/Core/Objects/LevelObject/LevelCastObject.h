// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LevelInteractionObject.h"
#include "LevelCastObject.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTT_API ALevelCastObject : public ALevelInteractionObject
{
	GENERATED_BODY()
	
public:
	virtual void TryInteraction(const FInteractionData InSendData, FOnInteractionSuccess Callback) override;
	virtual void SuccessPreAction_Implementation() override;
	virtual void SuccessUpdateAction_Implementation() override;
	virtual void SuccessCompleteAction_Implementation() override;	
	virtual void SetVisibility(bool InbVisible) override;

protected:
	virtual void ShowVisual_Implementation() override;
	virtual void HideVisual_Implementation() override;
	
	UFUNCTION(BlueprintNativeEvent)
	void SuccessInteraction(bool Result);

public:

protected:

private:
	uint8 bCompleteInteraction : 1;

};
