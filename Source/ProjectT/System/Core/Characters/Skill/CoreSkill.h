// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProjectT/Data/Gen/GenerateStructEffect.h"
#include "ProjectT/Data/Gen/GenerateStructSkill.h"
#include "ProjectT/System/Core/Interfaces/MouseInteraction.h"
#include "ProjectT/System/Core/Interfaces/PoolableActor.h"
#include "CoreSkill.generated.h"

DECLARE_DELEGATE(FSkillTimeEnd)
UCLASS()
class PROJECTT_API ACoreSkill : public AActor, public IPoolableActor, public IMouseInteraction
{
	GENERATED_BODY()

public:
	ACoreSkill();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void Enable_Implementation() override;
	virtual void Disable_Implementation() override;
	virtual void Pushed_Implementation() override;
	virtual void Obtained_Implementation() override;
	virtual void RefreshCursorHitResult_Implementation(FHitResult InHitResult) override;

	void InitializeSkillData();
	void SetSkillOwner(AActor* InSkillOwner);
	void GetSkillData(FSkill& OutCurrentSkillData);
	bool GetAimState() const { return bAimState; }
	bool GetActivated() const { return bActivated; }
	int32 GetSkillId() const { return SkillId; }
	int GetSkillPoolSize() const { return SkillPoolsize; }
	void SetCursorHitResult(const FHitResult& InHitResult) { CursorCalibrateHitResult = InHitResult; };
	float GetFinalDamage(AActor* InHitActor, bool InbLocationSelf = false);
	void SetParabolaLaunchVelocity();
	UPrimitiveComponent* GetBodyHitComponent() const;
	UPrimitiveComponent* GetRangeHitComponent() const;
	
	FORCEINLINE FSkill GetSkillData() const { return SkillData; }

	UFUNCTION(BlueprintCallable)
	virtual void StartSkill();

	UFUNCTION(BlueprintCallable)
	virtual void AimSkill();

	UFUNCTION(BlueprintCallable)
	virtual void ActivateSkill();

	UFUNCTION(BlueprintCallable)
	virtual void ReleaseSkill();
	
	UFUNCTION(BlueprintCallable)
	virtual void CancelSkill();
	
	UFUNCTION(BlueprintCallable)
	virtual void FinishSkill();

	UFUNCTION(BlueprintCallable)
	virtual void SkillHitEnable(bool InbEnable);

	UFUNCTION(BlueprintCallable)
	virtual void SkillObjectHitEnable(bool InbEnable);

	UFUNCTION(BlueprintCallable)
	virtual void SkillFXEnable(bool InbEnable, int32 InIndex = 0);
	
	UFUNCTION(BlueprintCallable)
	virtual void MakeSkillNoise(bool InbLocationSelf = false, UPrimitiveComponent* InAttachedComponent = nullptr);
	
	UFUNCTION(BlueprintCallable)
	virtual void ResetDefaultState();

	UFUNCTION(BlueprintCallable)
	void MovedComponentToHitPoint(AActor* InHitActor, UPrimitiveComponent* InMovedComponent);

public:
	FSkillTimeEnd SkillTimeEnd;
	
protected:
	ETraceTypeQuery LinearTraceQuery;
	FHitResult CursorCalibrateHitResult;
	FVector LaunchVelocity;
	FName SkillRowName;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Skill")
	TObjectPtr<USceneComponent> RootSceneComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Skill")
	TObjectPtr<class USphereComponent> NoiseSphereComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<class UNiagaraSystem> HitFXSystem;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<class UNiagaraComponent> NoiseFXComponent;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill")
	int SkillPoolsize;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill")
	FSkill SkillData;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill")
	TArray<FEffect> SkillEffects;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Skill")
	AActor* SkillOwner;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Skill")
	int32 SkillId;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Skill")
	bool bAimState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Skill")
	bool bCanceled;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Skill")
	bool bReleased;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Skill")
	bool bActivated;
	
	UPROPERTY()
	UPrimitiveComponent* BodyHitComponent;

	UPROPERTY()
	UPrimitiveComponent* RangeHitComponent;
};
