// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectT/System/Core/Characters/NPC/Animations/NpcAnimInstance.h"
#include "ProjectT/Data/Npc/NpcDataModel.h"
#include "SPNpcAgentAnimInstance.generated.h"


UCLASS()
class PROJECTT_API USPNpcAgentAnimInstance : public UNpcAnimInstance
{
	GENERATED_BODY()
	
public:
	USPNpcAgentAnimInstance();
	
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UFUNCTION(BlueprintCallable)
	void ChangeNpcAbnormalAnimState(const ENpcAbnormalAnimState InAbnormalState);
	
	UFUNCTION(BlueprintCallable)
	void ChangeNpcNormalAnimState(const ENpcNormalAnimState InNormalState);

	FORCEINLINE ENpcAbnormalAnimState GetNpcAbnormalAnimState() const { return CurrentAbnormalState; };
	void ResetAnimState();
	void SetActionMontage(UAnimMontage* InActionMontage);
	void SetNormalState(bool InNormal);
	void SetActionState(bool InAction);
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class ASPNpcAgent> OwnerNpcCharacter;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = NpcAnimationState)
	TObjectPtr<UAnimMontage> NpcActionMontage;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = NpcAnimationState)
	ENpcNormalAnimState CurrentNormalState;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = NpcAnimationState);
	ENpcNormalAnimState PrevNormalState;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = NpcAnimationState)
	ENpcAbnormalAnimState CurrentAbnormalState;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = NpcoAnimationState);
	uint8 bWalk : 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = NpcAnimationState);
	uint8 bRun : 1;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = NpcAnimationState);
	uint8 bNormal : 1;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = NpcAnimationState);
	uint8 bAction : 1;

};

