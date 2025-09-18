// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CoreSkill.h"
#include "IcienSkill.generated.h"

UCLASS()
class PROJECTT_API AIcienSkill : public ACoreSkill
{
	GENERATED_BODY()

public:
	AIcienSkill();
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
	void CheckActiveTime();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<USceneComponent> IcienSkillRootSceneComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<class UNiagaraComponent> IcienSkillFXComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UNiagaraComponent> IcienHandLFXComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UNiagaraComponent> IcienHandRFXComponent;

private:
	float CurrentActivateTime;
	
	UPROPERTY()
	FTimerHandle ActivateTimerHandle;
};
