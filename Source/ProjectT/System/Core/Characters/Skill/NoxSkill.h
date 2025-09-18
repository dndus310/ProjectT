// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CoreSkill.h"
#include "NoxSkill.generated.h"

UCLASS()
class PROJECTT_API ANoxSkill : public ACoreSkill
{
	GENERATED_BODY()

public:
	ANoxSkill();
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
	virtual void SkillFXEnable(bool InbEnable, int32 InIndex) override;
	virtual void ResetDefaultState() override;
	void CheckSurgeActivateTime();
	void CheckSurgeDeactivate();
	void RecoverySurgedNpc();

	UFUNCTION()
	void SkillAreaBeginOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	                           const FHitResult& SweepResult);

	UFUNCTION()
	void SkillAreaEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<USceneComponent> NoxSkillRootScene;
		
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<class UNiagaraComponent> WandFXComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UNiagaraComponent> SurgeFXComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<USphereComponent> NoxSkillHitCollisionComponent;

	UPROPERTY()
	TObjectPtr<UAudioComponent> SurgeSFXComponent;
	
	UPROPERTY()
	TArray<class ACoreNpc*> CurrentAffectNpc;
	
private:
	float SurgeWaveDelayMax;
	float SurgeWaveDelay;
	float AppearLength;
	float ActivateTime;
	float SurgeFXSwitchValue;
	FRotator SetRotation;
	
	UPROPERTY()
	FTimerHandle SurgeActivate;
		
	UPROPERTY()
	FTimerHandle SurgeFXDeactivate;
};
