// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CoreSkill.h"
#include "NoxAtk.generated.h"

UCLASS()
class PROJECTT_API ANoxAtk : public ACoreSkill
{
	GENERATED_BODY()

public:
	ANoxAtk();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void Enable_Implementation() override;
	virtual void Disable_Implementation() override;
	virtual void Pushed_Implementation() override;
	virtual void Obtained_Implementation() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

protected:
	virtual void StartSkill() override;
	virtual void CancelSkill() override;
	virtual void FinishSkill() override;
	virtual void SkillHitEnable(bool InbEnable) override;
	virtual void SkillFXEnable(bool InbEnable, int32 InIndex) override;
	virtual void ResetDefaultState() override;
	void ProjectileNoxBall();
	void ExplosionNoxBall();
	void CheckDistance();
	void CheckActiveBind();
	
	
	UFUNCTION()
	void DefaultCollisionBeginOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<USceneComponent> NoxAtkRootSceneComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<USphereComponent> NoxAtkHitCollisionComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<USphereComponent> NoxAtkEffectCollisionComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<class UNiagaraComponent> ThrowFXComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UNiagaraComponent> WandFXComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UNiagaraSystem> BindFXAsset;

	UPROPERTY()
	TObjectPtr<class UProjectileMovementComponent> ProjectileMovementComponent;

private:
	bool bHit;
	float BindTime;
	FVector InitLocation;
	
	UPROPERTY()
	FTimerHandle RangeCheckTimer;

	UPROPERTY()
	FTimerHandle BindEffectTimer;
	
	UPROPERTY()
	FTimerHandle BindCheckTimer;
};
