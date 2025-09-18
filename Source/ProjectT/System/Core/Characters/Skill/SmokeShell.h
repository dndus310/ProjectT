// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CoreSkill.h"
#include "SmokeShell.generated.h"

UCLASS()
class PROJECTT_API ASmokeShell : public ACoreSkill
{
	GENERATED_BODY()

public:
	ASmokeShell();
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
	void ProjectileSmokeShell();
	void ExplosionSmokeShell();
	void CheckActiveTime();
	void RotateSmokeMesh() const;
	void ReleaseEffects(AActor* InReleaseActor);
	void ReleaseNpc() const;
	
	UFUNCTION()
	void DefaultCollisionBeginOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void UnDetectCollisionBeginOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void UnDetectCollisionEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<USceneComponent> SmokeShellRootScene;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<class UNiagaraComponent> ThrowFXComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UNiagaraComponent> ExplosionFXComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<UStaticMeshComponent> SmokeShellMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<class USphereComponent> SmokeShellHitCollisionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<class UBoxComponent> UnDetectCollisionComponent;

	UPROPERTY()
	TObjectPtr<UAudioComponent> SmokeShellSFXComponent;
	
	UPROPERTY()
	TObjectPtr<class UProjectileMovementComponent> ProjectileMovementComponent;

private:
	TArray<class ACoreNpc*> CurrentOverlapNpc;
	FVector InitLocation;
	float CurrentActiveTime;
	bool bHit;
	
	UPROPERTY()
	FTimerHandle UnDetectTimerHandle;
	
	UPROPERTY()
	FTimerHandle RotatorTimerHandle;

	UPROPERTY()
	class ACorePlayerCharacter* CurrentOverlapPlayer;
};
