// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SPPlayerCharacter.h"
#include "SPSneako.generated.h"

UCLASS()
class PROJECTT_API ASPSneako : public ASPPlayerCharacter
{
	GENERATED_BODY()

public:
	ASPSneako();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void SetSkillState(EPlayerSkillState InSkillState, FSkill InSkillData = FSkill()) override;
	virtual void SetMeleeAtkSkillState(EPlayerSkillState InSkillState, FSkill InSkillData = FSkill()) override;

protected:
	UPROPERTY(EditDefaultsOnly)
	UMaterialInstance* HeadHideMaterialInstance;
	
	UPROPERTY(EditDefaultsOnly)
	UMaterialInstance* HeadMaterialInstance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = PlayerAnimation, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAnimSequence> SkillIdleAnimation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = PlayerAnimation, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAnimSequence> SkillMoveAnimation;
};
