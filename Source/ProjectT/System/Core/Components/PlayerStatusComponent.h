// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectT/Data/Gen/GenerateStructPlayerCharacter.h"
#include "Components/ActorComponent.h"
#include "PlayerStatusComponent.generated.h"
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUpdatePlayerHp, const float, HpPercent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUpdatePlayerStamina, const float, StaminaPercent);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTT_API UPlayerStatusComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPlayerStatusComponent();

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void InitializeStatusData(FPlayerCharacter InPlayerData);
	void ResetStatusDefault();
	bool ApplyDamage(float InApplyDamage);
	void SpendPlayerStamina(bool InbSpend);
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FPlayerCharacter GetPreviousPlayerStatus();
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FPlayerCharacter GetCurrentPlayerStatus();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetStatusPreviousCharHp();
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetStatusCharHp();

	UFUNCTION(BlueprintCallable)
	void SetStatusCharHp(float InSetValue);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetStatusPreviousCharMove();
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetStatusCharMove();

	UFUNCTION(BlueprintCallable)
	void SetStatusCharMove(float InSetValue);

	UFUNCTION(BlueprintCallable)
	void AddStatusCharMove(float InSetValue);	

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetStatusCharMoveSound();

	UFUNCTION(BlueprintCallable)
	void SetStatusCharMoveSound(float InSetValue);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetStatusPreviousCharRun();
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetStatusCharRun();

	UFUNCTION(BlueprintCallable)
	void SetStatusCharRun(float InSetValue);

	UFUNCTION(BlueprintCallable)
	void AddStatusCharRun(float InSetValue);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetStatusCharRunSound();

	UFUNCTION(BlueprintCallable)
	void SetStatusCharRunSound(float InSetValue);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetStatusPreviousStamina();
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetStatusStamina();

	UFUNCTION(BlueprintCallable)
	void SetStatusStamina(float InSetValue);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetStatusStaRecovery();

	UFUNCTION(BlueprintCallable)
	void SetStatusStaRecovery(float InSetValue);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PlayerStatus, meta = (AllowPrivateAccess = "true"))
	FPlayerCharacter PreviousPlayerStatus;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PlayerStatus, meta = (AllowPrivateAccess = "true"))
	FPlayerCharacter CurrentPlayerStatus;	

	UPROPERTY(BlueprintAssignable)
	FUpdatePlayerHp UpdatePlayerHpPercent;

	UPROPERTY(BlueprintAssignable)
	FUpdatePlayerStamina UpdatePlayerStaminaPercent;
};
