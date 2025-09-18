// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputAction.h"
#include "UObject/Object.h"
#include "PlayerDataModel.generated.h"

UENUM(BlueprintType)
enum class EPlayerSkillState: uint8
{
	None = 0 UMETA(DisplayName = "None"),
	Start UMETA(DisplayName = "Start"),
	Aim UMETA(DisplayName = "Aim"),
	Activate UMETA(DispalyName = "Activate"),
	Cancel UMETA(DIsplayeName = "Cancel"),
	Release UMETA(DisplayName = "Release"),
	Finish UMETA(DisplayName = "Finish")
};

USTRUCT(Blueprintable)
struct FPlayerActionTypes
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<UInputAction> AxisMoveAction;

	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<UInputAction> RunAction;

	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<UInputAction> MeleeAttackAction;
	
	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<UInputAction> SkillAction;
	
	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<UInputAction> InteractionAction;

	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<UInputAction> UseSlot1Action;

	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<UInputAction> UseSlot2Action;

	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<UInputAction> UseSlot3Action;

	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<UInputAction> MinimapAction;
		
	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<UInputAction> PauseAction;
};

USTRUCT(Blueprintable)
struct FPlayerChordedActionTypes
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<UInputAction> CtrlAction;
};

USTRUCT(Blueprintable)
struct FPlayerDebugActionTypes
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<UInputAction> DebugToggleCheat;

	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<UInputAction> DebugToggleInvincible;

	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<UInputAction> DebugTogglePlayerDamage;

	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<UInputAction> DebugTogglePlayerSpeed;

	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<UInputAction> DebugToggleFrustum;
	
	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<UInputAction> DebugToggleHitCollision;

	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<UInputAction> DebugToggleTimeStop;
	
	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<UInputAction> DebugToggleSlomo;
	
	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<UInputAction> DebugCharacterSelect;

	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<UInputAction> DebugToggleCam;

};

USTRUCT(Blueprintable)
struct FPlayerSimpleRatio
{
	GENERATED_BODY()
	FPlayerSimpleRatio() :
		CurrentValue(0.f),
		MaxValue(0.f)
	{		
	}
	
	UPROPERTY(EditAnywhere)
	float CurrentValue;

	UPROPERTY(EditAnywhere)
	float MaxValue;
};

USTRUCT(Blueprintable)
struct FPlayerEffectValue
{
	GENERATED_BODY()
	FPlayerEffectValue() :
		RemainTime(0.f),
		MaxTime(0.f),
		EffectValue(0.f)
	{		
	}
	
	UPROPERTY(EditAnywhere)
	float RemainTime;

	UPROPERTY(EditAnywhere)
	float MaxTime;

	UPROPERTY(EditAnywhere)
	float EffectValue;
};

UCLASS()
class PROJECTT_API UPlayerDataModel : public UObject
{
	GENERATED_BODY()
};
