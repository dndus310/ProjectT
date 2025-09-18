// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ObjectInteractionControl.generated.h"

UENUM(BlueprintType)
enum class EInteractionObjectType: uint8
{
	None = 0 UMETA(DisplayName = "None"),
	Item UMETA(DisplayName = "Item"),
	LevelObject UMETA(DisplayName = "LevelObject"),
	ObjectiveCharacter UMETA(DIsplayeName = "ObjectiveCharacter")
};

UENUM(BlueprintType)
enum class EInteractionResultType: uint8
{
	None = 0 UMETA(DisplayName = "None"),
	Fail UMETA(DisplayName = "Fail"),
	SuccessPre UMETA(DisplayName = "SuccessPre"),
	SuccessUpdate UMETA(DisplayName = "SuccessUpdate"),
	SuccessComplete UMETA(DisplayName = "SuccessComplete")
};

USTRUCT(Blueprintable)
struct FInteractionData
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Interaction")
	TArray<FName> RowNames;
};

UINTERFACE()
class UObjectInteractionControl : public UInterface
{
	GENERATED_BODY()
};

DECLARE_DELEGATE_OneParam(FOnInteractionSuccess, EInteractionResultType);
class PROJECTT_API IObjectInteractionControl
{
	GENERATED_BODY()

public:
	virtual void TryInteraction(const FInteractionData InSendData, FOnInteractionSuccess Callback = nullptr) = 0;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, meta = (Category = "CharacterInteraction"))
	void SuccessPreAction();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, meta = (Category = "CharacterInteraction"))
	void SuccessUpdateAction();
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, meta = (Category = "CharacterInteraction"))
	void SuccessCompleteAction();
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, meta = (Category = "ObjectInteraction"))
	EInteractionObjectType GetInteractionObjectType();

};
