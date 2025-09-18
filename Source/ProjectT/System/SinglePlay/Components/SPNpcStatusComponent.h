// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ProjectT/Data/Gen/GenerateStructEnemyNPC.h"
#include "SPNpcStatusComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNpcHit, float, HpRatio);
UCLASS(ClassGroup = (Component), meta = (BlueprintSpawnableComponent))
class PROJECTT_API USPNpcStatusComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USPNpcStatusComponent();
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void InitializeStatus(const FEnemyNpc& InStatus);
	void ReInitializeStatus();
	
	FORCEINLINE float GetCurrentHP() const { return CurrentNpcStatus.fNpcHp; }
	FORCEINLINE bool IsDead() const { return CurrentNpcStatus.fNpcHp <= 0.f; }
	bool ApplyDamage(const float InDamageAmount);
	void UpdateCurrentDoubtTime(const float InDoubtTime);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FEnemyNpc GetPreviousNpcStatus();
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FEnemyNpc GetCurrentNpcStatus();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetStatusPreviousNpcHp();
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetStatusNpcHp();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetStatusPreviousNpcMove();
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetStatusNpcMove();

	UFUNCTION(BlueprintCallable)
	void SetStatusNpcMove(const float InValue);

	UFUNCTION(BlueprintCallable)
	void AddStatusNpcMove(const float InValue);	

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetStatusPreviousNpcRun();
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetStatusNpcRun();

	UFUNCTION(BlueprintCallable)
	void SetStatusNpcRun(const float InValue);

	UFUNCTION(BlueprintCallable)
	void AddStatusNpcRun(const float InValue);
	
	UFUNCTION(BlueprintCallable)
	float GetStatusNpcSightLength();
	
	UFUNCTION(BlueprintCallable)
	float GetStatusNpcSightAngle();
	
	UFUNCTION(BlueprintCallable)
	float GetStatusNpcAttackLength();
	
	UFUNCTION(BlueprintCallable)
	float GetStatusNpcChaseLength();
	
	UFUNCTION(BlueprintCallable)
	float GetStatusNpcRotationAngle();

	UFUNCTION(BlueprintCallable)
	float GetStatusNpcHalfRotationAngle();
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetStatusNpcRotationSpeed();
	
	UFUNCTION(BlueprintCallable)
    void SetStatusNpcRotationSpeed(const float InValue);

	UFUNCTION(BlueprintCallable)
	float GetStatusNpcDoubtTime();

	UFUNCTION(BlueprintCallable)
	float GetStatusPreviousNpcDoubtTime();
	
	UFUNCTION(BlueprintCallable)
	float GetStatusNpcNoise();

	UFUNCTION(BlueprintCallable)
	float GetStatusNpcMoveProb();
	
	UFUNCTION(BlueprintCallable)
	float GetStatusNpcSearchProb();
	
	UFUNCTION(BlueprintCallable)
	float GetStatusNpcScoutProb();
	
	UFUNCTION(BlueprintCallable)
	int32 GetStatusNpcAttackId();
	
protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = NpcStatus, meta = (AllowPrivateAccess = "true"))
	FEnemyNpc PreviousNpcStatus;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = NpcStatus, meta = (AllowPrivateAccess = "true"))
	FEnemyNpc CurrentNpcStatus;

protected:
	UPROPERTY(BlueprintAssignable)
	FOnNpcHit OnNpcHit;
	
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = NpcStatus, meta = (AllowPrivateAccess = "true"))
	uint8 bOverrideStatusInEditor : 1;

	float HalfRotationAngle;
};
