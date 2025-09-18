// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectT/Data/Gen/GenerateEnumSkillTypes.h"
#include "ProjectT/Data/Npc/NpcDataModel.h"
#include "ProjectT/System/Core/Characters/NPC/CoreNpc.h"
#include "ProjectT/System/Core/Interfaces/AISettings.h"
#include "ProjectT/System/Core/Interfaces/DebugHitCollision.h"
#include "SPNpcAgent.generated.h"

enum class ENpcNormalAnimState : uint8;
enum class ENpcAgentAnimType : uint8;

UCLASS()
class PROJECTT_API ASPNpcAgent : public ACoreNpc, public IAISettings, public IDebugHitCollision
{
	GENERATED_BODY()

public:
	ASPNpcAgent();
	
	virtual void PostInitializeComponents() override;
	virtual void Tick(float DeltaTime) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual bool GetUseSight_Implementation() override;
	virtual bool GetUseHearing_Implementation() override;
	virtual FSightInfo GetSightInfo_Implementation() override;
	virtual UBlackboardData* GetBlackboardData_Implementation() override;
	virtual UBehaviorTree* GetBehaviorTree_Implementation() override;
	virtual void DetectTargetBySight_Implementation(AActor* InTarget, FAIStimulus InStimulus) override;
	virtual void ForgetTarget_Implementation(AActor* InTarget) override;
	virtual FTransform GetViewTransform_Implementation() override;
	virtual void ApplyDamageEvent_Implementation(AActor* InOccurActor, const FVector& InOriginLocation, const float InDamage, const TArray<FEffect>& InSkillEffects) override;
	virtual void ApplyRecoveryEvent_Implementation(const TArray<FEffect>& InReleaseEffects) override;
	virtual void StartDisableNpc() override;
	virtual void CompleteDisableNpc() override;
	virtual void ChangeNpcState(const ENpcState InState) override;
	virtual void ActivateCharacter(const bool InbEnable) override;
	virtual void SetDetectTargetFromNoise(AActor* InTarget) override;
	virtual void SetDetectLocationFromNoise(const FVector& InTargetLocation) override; 
	virtual void SetVisionMode(EVisionMode InVisionMode) override;
	virtual void ChangeDoubtState(const EDoubtState InDoubtState) override;
	
	UFUNCTION(BlueprintPure)
	class USPNpcAgentAnimInstance* GetAgentAnimInstance() const;

	UFUNCTION(BlueprintCallable)
	void ChangeSpeed(const ENpcSpeedType InSpeed, const bool InbStopImmediately = false);
	
	UFUNCTION()
	void FinishCheckInChaseRange();

	FORCEINLINE bool CanRotateWhileAttack() const { return bRotateWhileAttack; }
	class ACoreSkill* GetCurrentSkill() const;
	class AAIPerceptionController* GetAIPerceptionController() const;
	bool IsFinishSkillCoolDown() const;
	bool IsTargetInAttackRange(const AActor* InTarget) const;
	void GetSkillClass(TSubclassOf<AActor>& OutSkillClass) const;
	void PlayActionMontage(FName InSectionName, FOnMontageBlendingOutStarted InEndedFunction = nullptr);
	void StopSkillActionMontage();
	void TryAttack();
	bool ShouldContinueAttack() const;
	void DetermineNextStateAfterAbnormal();
	void SetCurrentUsingSkill(class ACoreSkill* InSkill);
	void FinishCurrentSkill();
	void SetSkillCoolTime(const float InCoolTime);

protected:
	virtual void BeginPlay() override;
	virtual void InitializeStatus(const FEnemyNpc InNpcStatus) override;
	virtual void InitializeProperty() override;
	virtual void Respawn() override;
	virtual void ExecuteClickedEvent(UPrimitiveComponent* InTouchedComponent, FKey InButtonPressed) override;
	virtual void ExecuteBeginCursorOverEvent(UPrimitiveComponent* InTouchedComponent) override;
	virtual void ExecuteEndCursorOverEvent(UPrimitiveComponent* InTouchedComponent) override;
	virtual void ResetStateAfterGameEnd() override;
	virtual void SetUnDetectTarget(const bool InbUnDetect) override;
	void InitializeMaterials();
	void StatusSlow(float InSlowValue, bool InbRecovery = false);
	void StatusFast(float InFastValue, bool InbRecovery = false);
	
#if WITH_EDITOR
	virtual void ApplyCharacterData(const FTableRowBase* RowData) override;
#endif
	
	UFUNCTION()
	void ContinueAttack();

	UFUNCTION()
	void TryFinishAttack();

	UFUNCTION()
	void FinishAttack();
	
	void RelocateOnNavMesh();
	void CheckTargetInChaseRange();
	void UpdateSkillCooldown(const float InDeltaSeconds);
	void JumpBack(const FVector& InTargetLocation);
	void CheckFallingFromJumpBack();
	void StartCheckInChaseRange();
	void SetSkillClass();
	void ClearSkillCoolTime();
	void StartAttack();
	void UpdateDissolve(const float InDeltaSeconds);
	void UpdateVisionLineLength();
	void NotifyVisionDetection(const bool InbDetect);

public:
	virtual void ShowVisual_Implementation() override;
	virtual void HideVisual_Implementation() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AISetting")
	FCrowdSettingsInfo CrowdSettingsInfo;
	
	uint8 bAttack : 1;

protected:
	ENpcSpeedType CurrentSpeed;
	ESkillTypes SkillType;
	FString ActionMontageSectionName;
	FName ChaseRangeProfileName;
	FName DissolveParameterName;
	float SkillCoolTime;
	float DissolveElapsedTime;
	float DissolveDuration;
	uint8 bRotateWhileAttack : 1;
	uint8 bSkillCooldownActive : 1;
	uint8 bDissolving : 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component")
	TObjectPtr<class UWidgetComponent> HpBarWidgetComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	TObjectPtr<class UWidgetComponent> DetectIndicatorWidget;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	TObjectPtr<class UObstacleFilterComponent> SphereChaseRangeComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Component")
	TObjectPtr<class UNiagaraComponent> DissolveFXComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component")
	TObjectPtr<class UStaticMeshComponent> VisionLineMeshComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
	TSubclassOf<class ACoreSkill> NpcSkillClass;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Skill")
	TObjectPtr<class ACoreSkill> CurrentUsingSkill;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
	TObjectPtr<class USPNpcAgentAnimInstance> AgentAnimInstance;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	TObjectPtr<class UAnimMontage> NpcActionMontage;
	
	UPROPERTY()
	FTimerHandle CheckChaseRangeTimerHandle;

	UPROPERTY()
	FTimerHandle JumpBackTimerHandle;

	UPROPERTY()
	TArray<TObjectPtr<UMeshComponent>> AllMeshComponents;

};
