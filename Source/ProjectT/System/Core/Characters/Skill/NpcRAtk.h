// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CoreSkill.h"
#include "NpcRAtk.generated.h"

UCLASS()
class PROJECTT_API ANpcRAtk : public ACoreSkill
{
	GENERATED_BODY()

public:
	ANpcRAtk();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;	
	virtual void Enable_Implementation() override;
	virtual void Disable_Implementation() override;
	virtual void Pushed_Implementation() override;
	virtual void Obtained_Implementation() override;
	virtual void SkillFXEnable(bool InbEnable, int32 InIndex = 0) override;

protected:
	virtual void StartSkill() override;
	virtual void ActivateSkill() override;
	virtual void FinishSkill() override;
	virtual void SkillHitEnable(bool InbEnable) override;
	virtual void ResetDefaultState() override;
	
	UFUNCTION()
	void DefaultCollisionBeginOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
private:
	void ShootProjectile();
	void CheckDistance();
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<class USphereComponent> DirectAttackCollisionComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> BulletMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<class UNiagaraComponent> ShootFXComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<class UNiagaraComponent> BulletTrailFXComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<class UNiagaraComponent> BulletHitFXComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<class UProjectileMovementComponent> ProjectileMovementComponent;
	
	UPROPERTY()
	TObjectPtr<class ASPNpcAgent> SkillNpcOwner;
	
	UPROPERTY()
	FTimerHandle RangeCheckTimerHandle;

	UPROPERTY()
	FTimerHandle ShootPoseCoolDownTimerHandle;
	
	FVector StartLocation;
	uint8 bHit : 1;

};

