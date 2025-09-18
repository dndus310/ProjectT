// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CoreSkill.h"
#include "SneakoSkill.generated.h"
UCLASS()
class PROJECTT_API ASneakoSkill : public ACoreSkill
{
	GENERATED_BODY()

public:
	ASneakoSkill();
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
	virtual void ReleaseSkill() override;
	virtual void CancelSkill() override;
	virtual void FinishSkill() override;
	virtual void SkillHitEnable(bool InbEnable) override;
	virtual void ResetDefaultState() override;
	void CheckActiveTime();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<USceneComponent> SneakoSkillRootSceneComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UStaticMeshComponent> SneakoSkillMeshComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<class UNiagaraComponent> SneakoSkillFXComponent;
	
	UPROPERTY()
	TObjectPtr<UAudioComponent> TransformSFXComponent;
	
private:
	float CurrentActivateTime;
	
	UPROPERTY()
	FTimerHandle ActivateTimerHandle;
};
