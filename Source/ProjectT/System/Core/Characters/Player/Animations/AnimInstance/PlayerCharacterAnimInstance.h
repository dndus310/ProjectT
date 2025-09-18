// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "PlayerCharacterAnimInstance.generated.h"
UENUM(BlueprintType)
enum class ENonInputAnimState: uint8
{
	None = 0 UMETA(DisplayName = "None"),
	Tired UMETA(DisplayName = "Tired"),
	HitBack UMETA(DisplayName = "HitBack"),
	JumpBackStart UMETA(DisplayName = "JumpBackStart"),
	JumpBackLoop UMETA(DisplayName = "JumpBackLoop"),
	JumpBackEnd UMETA(DisplayName = "JumpBackEnd"),
	GetUp UMETA(DisplayName = "GetUp"),
	Stun UMETA(DisplayName = "Stun"),
	Dead UMETA(DisplayName = "Dead"),
	Defeat UMETA(DisplayName = "Defeat"),
	Victory UMETA(DisplayName = "Victory")
};

UCLASS()
class PROJECTT_API UPlayerCharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UPlayerCharacterAnimInstance();
	void SetDefaultProperty(AActor* InOwner);
	void SetActionState(const bool InbActivate) { bAction = InbActivate; }
	void SetModifyState(const bool InbModify) { bModify = InbModify; }
	void NonInputAnimFinished();
	void GetCommonMontage(UAnimMontage*& OutCommonMontage) const { OutCommonMontage = PlayerCommonMontage; }
	void SetModifyAnimation(const TObjectPtr<UAnimSequence>& InIdleSequence = nullptr, const TObjectPtr<UAnimSequence>& InMoveSequence = nullptr,
	                        const TObjectPtr<UAnimSequence>& InRunSequence = nullptr);
	ENonInputAnimState GetCurrentNonInputState() { return NonInputAnimState; }

	FORCEINLINE ENonInputAnimState GetCurrentNonInputState() const { return NonInputAnimState; }
	

	UFUNCTION(BlueprintCallable)
	void SetActionMontage(UAnimMontage* InSetActionMontage);

	UFUNCTION(BlueprintCallable)
	void SetNonInputAnimState(ENonInputAnimState InSetState);

	UFUNCTION(BlueprintCallable)
	void StateChangeTest();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class ACorePlayerCharacter* OwnerPlayerCharacter;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PlayerAnimationState)
	bool bPlayerInputable;
		
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PlayerAnimationState)
	bool bIdle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PlayerAnimationState)
	bool bRest;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PlayerAnimationState)
	bool bWalk;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PlayerAnimationState)
	bool bRun;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PlayerAnimationState)
	bool bAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PlayerAnimationState)
	bool bModify;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PlayerAnimationState)
	TObjectPtr<UAnimMontage> PlayerActionMontage;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PlayerAnimationState)
	TObjectPtr<UAnimMontage> PlayerCommonMontage;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PlayerAnimationState)
	TObjectPtr<UAnimSequence> PlayerIdleAnimation;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PlayerAnimationState)
	TObjectPtr<UAnimSequence> PlayerRestAnimation;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PlayerAnimationState)
	TObjectPtr<UAnimSequence> PlayerMoveAnimation;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PlayerAnimationState)
	TObjectPtr<UAnimSequence> PlayerRunAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PlayerAnimationState)
	TObjectPtr<UAnimSequence> ModifyIdleAnimation;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PlayerAnimationState)
	TObjectPtr<UAnimSequence> ModifyMoveAnimation;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PlayerAnimationState)
	TObjectPtr<UAnimSequence> ModifyRunAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PlayerAnimationState)
	ENonInputAnimState NonInputAnimState;
};
