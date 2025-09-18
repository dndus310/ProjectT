// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CoreSkill.h"
#include "IcienAtk.generated.h"

UCLASS()
class PROJECTT_API AIcienAtk : public ACoreSkill
{
	GENERATED_BODY()

public:
	AIcienAtk();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void Enable_Implementation() override;
	virtual void Disable_Implementation() override;
	virtual void Pushed_Implementation() override;
	virtual void Obtained_Implementation() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
protected:
	virtual void StartSkill() override;
	virtual void ActivateSkill() override;
	virtual void CancelSkill() override;
	virtual void FinishSkill() override;
	virtual void SkillHitEnable(bool InbEnable) override;
	virtual void SkillFXEnable(bool InbEnable, int32 InIndex = 0) override;
	virtual void ResetDefaultState() override;
	void CheckFXDeactivate();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<USceneComponent> IcienAtkRootScene;
			
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<USphereComponent> IcienAtkHitCollisionComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<class UNiagaraComponent> IcienAtkFXComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UNiagaraComponent> IcienHandLFXComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UNiagaraComponent> IcienHandRFXComponent;

	UPROPERTY()
	TObjectPtr<UAudioComponent> AtkSFXComponent;
		
private:
	FVector ActivateLocation;
	FRotator SetRotation;
	
	UPROPERTY()
	FTimerHandle FXDeactivate;
};
