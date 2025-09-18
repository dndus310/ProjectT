// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CoreSkill.h"
#include "HealingPotion.generated.h"

UCLASS()
class PROJECTT_API AHealingPotion : public ACoreSkill
{
	GENERATED_BODY()

public:
	AHealingPotion();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void ActivateSkill() override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UNiagaraSystem> HealFXAsset;

	UPROPERTY()
	TObjectPtr<UAudioComponent> HealSFXComponent;
};
