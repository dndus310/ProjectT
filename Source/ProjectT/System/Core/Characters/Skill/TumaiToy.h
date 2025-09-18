// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CoreSkill.h"
#include "TumaiToy.generated.h"

UCLASS()
class PROJECTT_API ATumaiToy : public ACoreSkill
{
	GENERATED_BODY()

public:
	ATumaiToy();
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
	virtual void SkillObjectHitEnable(bool InbEnable) override;
	virtual void ResetDefaultState() override;
	void ProjectileTumaiToy();
	void ExplosionTumaiToy();
	void RotateTumaiToyMesh() const;
	void CheckFXDeactivated();
	void CheckDissolveDeactivated();
	
	UFUNCTION()
	void DefaultCollisionBeginOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<USceneComponent> TumaiToyRootScene;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<USceneComponent> MeshRootSceneComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<USkeletalMeshComponent> TumaiToyMeshComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<USphereComponent> TumaiToyHitCollisionComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TObjectPtr<UAnimSequence> TumaiToyIdleAnimSequence;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TObjectPtr<UAnimMontage> TumaiToyMontage;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	TObjectPtr<UMaterialInstance> DissolveMaterialInstance;

	UPROPERTY()
	TObjectPtr<UAudioComponent> TumaiToySFXComponent;

	UPROPERTY()
	TObjectPtr<class UProjectileMovementComponent> ProjectileMovementComponent;
	
private:
	FVector InitLocation;
	float CurrentActiveTime;
	float DissolveValue;
	bool bHit;
			
	UPROPERTY()
	FTimerHandle RotatorTimerHandle;

	UPROPERTY()
	FTimerHandle FXDeactivateTimerHandle;

	UPROPERTY()
	FTimerHandle DissolveTimerHandle;
};
