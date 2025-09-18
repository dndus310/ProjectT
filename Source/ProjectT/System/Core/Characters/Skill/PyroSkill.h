// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CoreSkill.h"
#include "PyroSkill.generated.h"

UCLASS()
class PROJECTT_API APyroSkill : public ACoreSkill
{
	GENERATED_BODY()

public:
	APyroSkill();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void Enable_Implementation() override;
	virtual void Disable_Implementation() override;
	virtual void Pushed_Implementation() override;
	virtual void Obtained_Implementation() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
protected:
	virtual void StartSkill() override;
	virtual void AimSkill() override;
	virtual void ActivateSkill() override;
	virtual void ReleaseSkill() override;
	virtual void CancelSkill() override;
	virtual void FinishSkill() override;
	virtual void SkillHitEnable(bool InbEnable) override;
	virtual void ResetDefaultState() override;
	void ProjectileFireBall();
	void ExplosionFireBall();
	void CheckDistance();
	void CheckActiveExplosion();
	
	UFUNCTION()
	void DefaultCollisionBeginOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<USceneComponent> FireBallRootScene;
		
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<class UNiagaraComponent> ThrowFXComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UNiagaraComponent> ExplosionFXComponent;
		
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<UStaticMeshComponent> FireBallMeshComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<USphereComponent> FireBallHitCollisionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<USphereComponent> FireBallEffectCollisionComponent;

	UPROPERTY()
	TObjectPtr<UAudioComponent> ThrowSFXComponent;

	UPROPERTY()
	TObjectPtr<UAudioComponent> ExplosionSFXComponent;	
	
	UPROPERTY()
	TObjectPtr<class UProjectileMovementComponent> ProjectileMovementComponent;
	
private:
	bool bHit;
	FVector InitLocation;
	
	UPROPERTY()
	FTimerHandle RangeCheckTimer;
	
	UPROPERTY()
	FTimerHandle ExplosionCheckTimer;
};
