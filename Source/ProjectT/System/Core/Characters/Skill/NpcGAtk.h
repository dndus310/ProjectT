// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CoreSkill.h"
#include "NpcGAtk.generated.h"

UCLASS()
class PROJECTT_API ANpcGAtk : public ACoreSkill
{
	GENERATED_BODY()

public:
	ANpcGAtk();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void Enable_Implementation() override;
	virtual void Disable_Implementation() override;
	virtual void Pushed_Implementation() override;
	virtual void Obtained_Implementation() override;
	virtual void SkillFXEnable(bool InbEnable, int32 InIndex = 0) override;

protected:
	virtual void StartSkill() override;
	virtual void FinishSkill() override;
	virtual void SkillHitEnable(bool InbEnable) override;
	virtual void ResetDefaultState() override;
	
	UFUNCTION()
	void DefaultCollisionBeginOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<class UBoxComponent> MeleeAttackCollisionComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<class UNiagaraComponent> MeleeSwingFXComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<class UNiagaraComponent> MeleeHitFXComponent;
	
	UPROPERTY()
	TObjectPtr<class ASPNpcAgent> SkillNpcOwner;
};
