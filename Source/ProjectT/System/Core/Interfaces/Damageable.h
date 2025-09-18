// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectT/Data/Gen/GenerateEnumEffectSubTypes.h"
#include "ProjectT/Data/Gen/GenerateStructEffect.h"
#include "ProjectT/Data/Gen/GenerateStructSkill.h"
#include "UObject/Interface.h"
#include "Damageable.generated.h"


// This class does not need to be modified.
UINTERFACE()
class UDamageable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PROJECTT_API IDamageable
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, meta = (Category = "Damageable"))
	void ApplyDamageEvent(AActor* InOccurActor, const FVector& InOriginLocation, const float InDamage, const TArray<FEffect>& InSkillEffects);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, meta = (Category = "Damageable"))
	void ApplyRecoveryEvent(const TArray<FEffect>& InReleaseEffects);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, meta = (Category = "Damageable"))
	void GetCurrentEffectTypes(TArray<EEffectSubTypes>& OutEffectSubTypes);
};
