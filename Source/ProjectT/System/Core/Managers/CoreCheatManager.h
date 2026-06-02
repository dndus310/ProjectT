// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "CoreCheatManager.generated.h"


USTRUCT(BlueprintType)
struct FDebugStatus
{
	GENERATED_BODY()

	FDebugStatus():
		bDebugPlayerInvincibleHP(0),
		bDebugPlayerDamage(0),
		bDebugPlayerSpeed(0),
		bDebugPlayerInvincibleSkill(0),
		bDebugPlayerInvincibleStatusEffect(0),
		bDebugEnemyInvincibleHP(0),
		bDebugEnemyDamage(0),
		bDebugEnemySpeed(0),
		bDebugEnemyInvincibleSkill(0),
		bDebugEnemyInvincibleStatusEffect(0),
		bDebugVision(0),
		bDebugHitCollision(0),
		bDebugTimeStop(0),
		bDebugSlomo(0)
	{
	}

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	uint8 bDebugPlayerInvincibleHP : 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	uint8 bDebugPlayerDamage : 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	uint8 bDebugPlayerSpeed : 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	uint8 bDebugPlayerInvincibleSkill : 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	uint8 bDebugPlayerInvincibleStatusEffect : 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	uint8 bDebugEnemyInvincibleHP : 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	uint8 bDebugEnemyDamage : 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	uint8 bDebugEnemySpeed : 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	uint8 bDebugEnemyInvincibleSkill : 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	uint8 bDebugEnemyInvincibleStatusEffect : 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	uint8 bDebugVision : 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	uint8 bDebugHitCollision : 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	uint8 bDebugTimeStop : 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	uint8 bDebugSlomo : 1;
};

DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnDebugSPGame, const FName&, StatusName, const bool, bState);

UCLASS()
class PROJECTT_API UCoreCheatManager : public UObject
{
	GENERATED_BODY()

public:
	UCoreCheatManager();

	UFUNCTION(BlueprintCallable)
	void BroadCastDebugStatus();

	UFUNCTION(BlueprintCallable)
	void BindOnDebugSPGame(const FOnDebugSPGame& Callback);
	void RefreshStatusEvent();

	UFUNCTION(BlueprintCallable, Category = "Debug")
	void DebugToggleCheat();

	UFUNCTION(BlueprintCallable, Category = "Debug")
	void DebugResetStatus();

	UFUNCTION(BlueprintCallable, Category = "Debug")
	void DebugRestartGame() const;

	UFUNCTION(BlueprintCallable, Category = "Debug")
	void DebugDrawFrustum() const;

	UFUNCTION(BlueprintCallable, Category = "Debug")
	void DebugTogglePlayerInvincibleHP();

	UFUNCTION(BlueprintCallable, Category = "Debug")
	void DebugTogglePlayerDamage();

	UFUNCTION(BlueprintCallable, Category = "Debug")
	void DebugTogglePlayerSpeed();

	UFUNCTION(BlueprintCallable, Category = "Debug")
	void DebugTogglePlayerInvincibleStatusEffect();

	UFUNCTION(BlueprintCallable, Category = "Debug")
	void DebugTogglePlayerInvincibleSkill();

	UFUNCTION(BlueprintCallable, Category = "Debug")
	void DebugToggleEnemyInvincibleHP();

	UFUNCTION(BlueprintCallable, Category = "Debug")
	void DebugToggleEnemyDamage();

	UFUNCTION(BlueprintCallable, Category = "Debug")
	void DebugToggleEnemySpeed();

	UFUNCTION(BlueprintCallable, Category = "Debug")
	void DebugToggleEnemyInvincibleStatusEffect();

	UFUNCTION(BlueprintCallable, Category = "Debug")
	void DebugToggleEnemyInvincibleSkill();

	UFUNCTION(BlueprintCallable, Category = "Debug")
	void DebugToggleVision();

	UFUNCTION(BlueprintCallable, Category = "Debug")
	void DebugToggleHitCollision();

	UFUNCTION(BlueprintCallable, Category = "Debug")
	void DebugToggleTimeStop();

	UFUNCTION(BlueprintCallable, Category = "Debug")
	void DebugToggleSlomo();

	UFUNCTION(BlueprintCallable, Category = "Debug")
	void DebugSpectatorPlay();

	UFUNCTION(BlueprintCallable, Category = "Debug")
	void DebugAddMainQuest();

	UFUNCTION(BlueprintCallable, Category = "Debug")
	void DebugAddSubQuest(const int32 QuestIndex);

	UFUNCTION(BlueprintCallable, Category = "Debug")
	FDebugStatus GetDebugStatus();

	void InitializeManager(const UObject* WorldObjectContext);
	void ReleaseManager();

private:
	void PropagateStatusEvent(const bool State);

private:
	UPROPERTY()
	TObjectPtr<UObject> OuterObject;

	UPROPERTY()
	FTimerHandle DebugTimerHandle;

	/**
	 * NOTE: FDebugStatus의 멤버 필드명에 따라 멤버함수를 적재함
	 * DebugStatus의 따라 동작하는 함수 추가시 멤버함수명은 FDebugStatus의 BodyName을 따라가야함
	 */
	UPROPERTY()
	TMap<FName, UFunction*> CheatFunctionMap;

	UPROPERTY()
	FOnDebugSPGame OnDebugSPGame;

	UPROPERTY()
	TObjectPtr<UUserWidget> DebugWidget;

	ESlateVisibility DebugWidgetVisibility;

	uint8 bDebugToggleCheat : 1;
	uint8 bInitialized : 1;

	FDebugStatus DebugStatus;
	FDebugStatus SaveDebugStatus;
};
