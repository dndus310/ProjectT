// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectT/ProjectT.h"
#include "ProjectT/Data/Gen/GenerateEnumContentType.h"
#include "ProjectT/Data/Gen/GenerateStructEnemyNPC.h"
#include "ProjectT/Data/Npc/NpcDataModel.h"
#include "ProjectT/System/Core/Characters/CombatCharacter.h"
#include "ProjectT/System/Core/Interfaces/Damageable.h"
#include "ProjectT/System/Core/Interfaces/PoolableActor.h"
#include "CoreNpc.generated.h"

struct FEnemyNpc;

UENUM(BlueprintType)
enum class EHeadRotationPhase : uint8
{
	TurnForward = 0 UMETA(DisplayName = "TurnForward"),
	TurnRight UMETA(DisplayName = "TurnRight"),
	TurnLeft UMETA(DisplayName = "TurnLeft"),
};

UENUM(BlueprintType)
enum class ENpcIndicatorState : uint8
{
	None = 0 UMETA(DisplayName = "None"),
	Hide UMETA(DisplayName = "Hide"),
	Question UMETA(DisplayName = "Question"),
	Exclamation UMETA(DisplayName = "Exclamation"),
};

USTRUCT(BlueprintType)
struct FHeadRotationInfo
{
	GENERATED_BODY()
	
	FHeadRotationInfo() :
		bOverrideHeadRotation(false),
		bChangeAlpha(false),
		TargetRotation(FRotator::ZeroRotator),
		HeadRotationType(EHeadRotationType::None),
		CurrentPhaseIdx(0),
		RotationSpeed(0.f),
		RotationHalfAngle(0.f),
		TargetAlpha(0.f)
	{}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 bOverrideHeadRotation : 1;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	uint8 bChangeAlpha;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FRotator TargetRotation;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EHeadRotationType HeadRotationType;
	
	UPROPERTY(BlueprintReadOnly)
	int32 CurrentPhaseIdx;
	
	UPROPERTY(BlueprintReadOnly)
	float RotationSpeed;

	UPROPERTY(BlueprintReadOnly)
	float RotationHalfAngle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float TargetAlpha;

};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnNpcDead);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnNpcRespawn);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNpcIndicatorChanged, ENpcIndicatorState, IndicatorState);
UCLASS()
class PROJECTT_API ACoreNpc : public ACombatCharacter, public IPoolableActor, public IDamageable
{
	GENERATED_BODY()
	CONTENT_TYPE_NUMBER(EContentType::EnemyNpc)
	
public:
	ACoreNpc();
	
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void Tick(float DeltaTime) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void GetActorEyesViewPoint( FVector& Location, FRotator& Rotation) const override;
	virtual void PostInitializeComponents() override;
	virtual void Enable_Implementation() override;
	virtual void Disable_Implementation() override;
	virtual void Pushed_Implementation() override;
	virtual void Obtained_Implementation() override;
	virtual void GetCurrentEffectTypes_Implementation(TArray<EEffectSubTypes>& OutEffectSubTypes) override;
	virtual void CompleteDisableNpc();
	virtual void ActivateCharacter(const bool InbEnable);
	virtual void SetDetectTargetFromNoise(AActor* InTarget);
	virtual void SetDetectLocationFromNoise(const FVector& InTargetLocation);
	virtual void SetVisionMode(EVisionMode InSightMode);
	virtual void UpdateDoubtTime(const float InCurrentDoubtTime);
	virtual void ChangeDoubtState(const EDoubtState InDoubtState);
	void UpdateDoubtVisualize(const float InCurrentDoubtTime);

	UFUNCTION(BlueprintCallable)
	virtual void StartDisableNpc();
	
	UFUNCTION(BlueprintCallable)
	class USPNpcStatusComponent* GetNpcStatusComponent() const;
	
	UFUNCTION(BlueprintPure)
	FORCEINLINE ENpcState GetNpcState() const { return CurrentState; }

	UFUNCTION(BlueprintPure)
	FORCEINLINE ENpcState GetNpcPrevState() const { return PrevState; }

	UFUNCTION(BlueprintPure)
	FORCEINLINE FVector GetRespawnLocation() const { return DefaultLocation; }
	
	UFUNCTION(BlueprintCallable)
	void ChangeHeadRotationState(const EHeadRotationType InHeadRotationType);
	
	UFUNCTION(BlueprintCallable)
	virtual void ChangeNpcState(const ENpcState InState);

	FORCEINLINE bool GetCanTriggerAlert() const { return bCanTriggerAlert; }
	FORCEINLINE bool GetActivatedOnStart() { return bActivatedOnStart; }
	FORCEINLINE void SetActivatedOnStart(bool InbActivate) { bActivatedOnStart = InbActivate; }
	FORCEINLINE EDoubtState GetCurrentDoubtState() const { return CurrentDoubtState; }
	FORCEINLINE EVisionMode GetCurrentVisionMode() const { return CurrentVisionMode; }
	FORCEINLINE EHeadRotationType GetHeadRotationType() const { return HeadRotationInfo.HeadRotationType; }
	
	void UpdateActiveComponentInFrustum() const;
	void SetCanTriggerAlert(const bool InbCanTrigger) { bCanTriggerAlert = InbCanTrigger; }
	void SetHeadRotationEnable(const bool InbEnable);
	bool IsAggressiveState(const ENpcState InState) const;
	bool IsDamagedState(const ENpcState InState) const;
	void Alert(const FVector& InTargetLocation);
	void HandleForceDetectFromNoise(const bool InbDetectTarget);
	FEnemyNpc GetCurrentNpcStatus() const;
	class AAIControllerBase* GetAIControllerBase() const;
	bool CanDetectTargetFromNoise() const;
	void ApplyStateToQuest(const ENpcState InState);
	void ChangeToChaseInDoubtGauge();
	void DebugToggleFrustum();
	void SetHeadTargetRotation(const FVector& InTargetLocation);
	TArray<EEffectSubTypes> GetCurrentEffects() const;
	
protected:
	virtual void BeginPlay() override;
	
#if WITH_EDITOR
	virtual void ApplyCharacterData(const FTableRowBase* RowData) override;
#endif
	
	virtual void InitializeStatus(const FEnemyNpc InNpcStatus);
	virtual void InitializeProperty();
	virtual void InitializeEvents();
	virtual void Respawn() override;
	virtual void ResetStateAfterGameEnd();
	virtual void SetSpringArmRotation();
	virtual void SetUnDetectTarget(const bool InbUnDetect);
	virtual void ChangeIndicatorState(const ENpcState InState);
	void SetupHeadRotationPattern();
	void UpdateHeadRotation(float InDeltaTime);
	void UpdateHeadRotationAlpha(float InDeltaTime);
	void ResetHeadRotation();
	void PostAlert() const;
	bool CheckEffectTime(const FEffect& InEffect);
	void AddEffectTimeMap(const FEffect& InEffect);
	void EffectTimeDown();
	void SetReturnLocationFromDefault() const;

public:
	UPROPERTY(BlueprintAssignable)
	FOnNpcDead OnNpcDead;

	UPROPERTY(BlueprintAssignable)
	FOnNpcRespawn OnNpcRespawn;

	UPROPERTY(BlueprintAssignable)
	FOnNpcIndicatorChanged OnNpcIndicatorChanged;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector DefaultLocation;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator DefaultRotation;
	
	uint8 bReachedHeadRotation : 1;
	uint8 bCanRotateHeadToTarget : 1;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AISetting")
	uint8 bEnableRespawn : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AISetting", meta = (EditCondition = "bEnableRespawn", EditConditionHides))
	float RespawnTime;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AISetting")
	TObjectPtr<class UBehaviorTree> BehaviorTree;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AISetting")
	TObjectPtr<class UBlackboardData> Blackboard;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component")
	TObjectPtr<class USceneComponent> SubRootComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component")
	TObjectPtr<class USceneComponent> WidgetRootComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component")
	TObjectPtr<class USpringArmComponent> SpringArmComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component")
	TObjectPtr<class USceneComponent> VisionHead;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component")
	TObjectPtr<class UVisionConeComponent> VisionConeComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	TObjectPtr<class USPNpcStatusComponent> NpcStatusComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	TObjectPtr<class UObstacleFilterComponent> AlertComponent;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Animation")
	FHeadRotationInfo HeadRotationInfo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<class UNiagaraComponent> NoiseFXComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component", meta = (AllowPrivateAccess = "true"))
	ENpcState CurrentState;

	UPROPERTY(EditAnywhere)
	float PerceptionMaxAge;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	ENpcIndicatorState LastIndicatorState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PlayerStatus, meta = (AllowPrivateAccess = "true"))
	TMap<EEffectSubTypes, FNpcEffectValue> EffectTimeMap;
	
	UPROPERTY()
	TObjectPtr<class UNpcAnimInstance> NpcAnimInstance;
	
	UPROPERTY()
	TArray<EHeadRotationPhase> CurrentPhaseLists;
	
	UPROPERTY()
	FTimerHandle UpdateActiveComponentHandle;

	UPROPERTY()
	FTimerHandle RespawnTimerHandle;

	UPROPERTY()
	FTimerHandle EffectTimerHandle;

	UPROPERTY()
	FTimerHandle CompleteAlertTimerHandle;
	
	UPROPERTY()
	uint8 bActivatedOnStart : 1;

	ENpcState PrevState;
	EDoubtState CurrentDoubtState;
	EVisionMode CurrentVisionMode;
	FName EyeSocketName;
	FName AlertSphereProfileName;	
	TMap<EHeadRotationType, TArray<EHeadRotationPhase>> HeadRotationPatterns;
	float LoseSightOffset;
	float HeadRotSpeedMultiplier;
	uint8 bCanTriggerAlert : 1;
	uint8 bUnDetect : 1;
	uint8 bFrustumCullingTimerSet : 1;
	uint8 bActivate : 1;
	uint8 bUseSocketTransformForVision : 1;

};
