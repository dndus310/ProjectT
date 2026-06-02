// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "ProjectT/System/Core/Characters/CombatCharacter.h"
#include "ProjectT/ProjectT.h"
#include "ProjectT/Data/Gen/GenerateEnumContentType.h"
#include "ProjectT/Data/Gen/GenerateStructPlayerCharacter.h"
#include "ProjectT/System/Core/Interfaces/Damageable.h"
#include "ProjectT/Data/Player/PlayerDataModel.h"
#include "CorePlayerCharacter.generated.h"

struct FInputActionValue;
class UInputAction;
class UInputMappingContext;
class UCoreCameraComponent;
class USpringArmComponent;
DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnRequestInteractIcon, const FString&, ThumnailPath, const bool, bVisibility);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FItemSlotActivate, const int&, SlotNumber, const bool, bActivate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FSkillCoolTime, TSubclassOf<class ACoreSkill>, SkillClass, float, RemainTimeRatio, bool, bCoolDownComplete);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FEffectTime, EEffectSubTypes, EffectSubTypes, float, RemainTimeRatio, bool, bEffectRecover);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInteractionIconVisibility, bool, bVisible);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPlayerDeadDone);

UCLASS()
class PROJECTT_API ACorePlayerCharacter : public ACombatCharacter, public IGenericTeamAgentInterface, public IDamageable
{
	GENERATED_BODY()
	CONTENT_TYPE_NUMBER(EContentType::PlayerCharacter)

public:
	ACorePlayerCharacter();
	virtual FGenericTeamId GetGenericTeamId() const override { return FGenericTeamId(1); }
	virtual void PostLoad() override;
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void ApplyDamageEvent_Implementation(AActor* InOccurActor, const FVector& InOriginLocation, const float InDamage, const TArray<FEffect>& InSkillEffects) override;
	virtual void ApplyRecoveryEvent_Implementation(const TArray<FEffect>& InReleaseEffects) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	void SetupPlayerInputActions();
	void SetCurrentSkill(bool InbSet, ACoreSkill* InSetSkill = nullptr);
	void PlayActionMontage(FName InSectionName, FOnMontageEnded InEndedFunction = nullptr, FOnMontageBlendingOutStarted InBlendOutFunction = nullptr) const;
	void PlayCommonMontage(FName InSectionName, FOnMontageEnded InEndedFunction = nullptr, FOnMontageBlendingOutStarted InBlendOutFunction = nullptr) const;
	void StopActionMontage() const;
	void SetUnDetectState(bool InbUnDetect);
	void SetSlotCursor(int InSetSlotCursor);

	void GetAudioListener(USceneComponent*& OutAudioListener);
	void GetCameraComponent(TObjectPtr<UCoreCameraComponent>& OutCoreCameraComponent) const { OutCoreCameraComponent = CoreCameraComponent; }
	void GetAnimInstance(class UPlayerCharacterAnimInstance*& OutAnimInstance) const { OutAnimInstance = PlayerAnimInstance; }
	void GetCurrentSkill(ACoreSkill*& OutCurrentSkill) const { OutCurrentSkill = CurrentUsingSkill; }
	void GetInvincible(bool& OutbInvincible) const { OutbInvincible = bInvincible; }
	void GetSkillClass(TSubclassOf<AActor>& OutAtkSkillClass, TSubclassOf<AActor>& OutSkillClass) const;
	void GetActionMontage(UAnimMontage*& OutActionMontage) const { OutActionMontage = ActionMontage; }
	void GetIdleAnimSequence(TObjectPtr<UAnimSequence>& OutIdleAnimSequence) const { OutIdleAnimSequence = IdleAnimation; }
	void GetRestAnimSequence(TObjectPtr<UAnimSequence>& OutIdleAnimSequence) const { OutIdleAnimSequence = RestAnimation; }	
	void GetMoveAnimSequence(TObjectPtr<UAnimSequence>& OutMoveAnimSequence) const { OutMoveAnimSequence = MoveAnimation; }
	void GetRunAnimSequence(TObjectPtr<UAnimSequence>& OutRunAnimSequence) const { OutRunAnimSequence = RunAnimation; }
	void GetPlayerCharacterData(FPlayerCharacter& OutPlayerCharacterData) const;
	void GetUnDetectState(bool& OutUnDetect) const { OutUnDetect = bUnDetect; }
	void GetSlotCursor(int& OutCurrentCursor) const { OutCurrentCursor = CurrentSlotCursor; }
	FORCEINLINE class UPlayerInventoryComponent* GetPlayerInventory() { return PlayerInventoryComponent; } 
	FORCEINLINE class UPlayerStatusComponent* GetPlayerStatus() { return PlayerStatusComponent; }

	UFUNCTION(BlueprintCallable)
	void SetInputable(bool InbEnable);

	UFUNCTION(BlueprintCallable)
	void SetInvincible(bool InbInvincible);

	UFUNCTION(BlueprintNativeEvent)
	void StartCountDown();

	UFUNCTION()
	void EventOnDead();

	UFUNCTION(BlueprintCallable)
	void UpdateRequestInteractionIcon(const FOnRequestInteractIcon& Callback);

protected:
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual void ApplyCharacterData(const FTableRowBase* RowData) override;
#endif
	virtual void SetSkillState(EPlayerSkillState InSkillState, FSkill InSkillData = FSkill());
	virtual void SetMeleeAtkSkillState(EPlayerSkillState InSkillState, FSkill InSkillData = FSkill());
	virtual void SetItemSkillState(EPlayerSkillState InSkillState, FSkill InSkillData = FSkill());
	void InitPlayerCharacter();
	void InitializeProperty();
	void SetSkillClass();
	
	void AxisMoveStarted();
	void AxisMoveTriggered(const FInputActionValue& Value);
	void AxisMoveCompleted();
	void RunTriggered(const FInputActionValue& Value);
	virtual void ChangeValueOnRun(bool InbRun);
	void RunCompleted();
	void ActivateMeleeAttack();
	void ActivateSkill();
	void ActivateItemSkill(int InCurrentInput, int InItemSkillId);
	void InteractionStarted();
	void InteractionCompleted();
	void UseItemSlot1();
	void UseItemSlot2();
	void UseItemSlot3();
	void ToggleGameMenu();

	void DebugToggleCheat(const FInputActionValue& InputActionValue);
	void DebugToggleInvincible(const FInputActionValue& InputActionValue);
	void DebugTogglePlayerDamage(const FInputActionValue& InputActionValue);
	void DebugTogglePlayerSpeed(const FInputActionValue& InputActionValue);
	void DebugToggleFrustum(const FInputActionValue& InputActionValue);
	void DebugToggleHitCollision(const FInputActionValue& InputActionValue);
	void DebugToggleTimeStop(const FInputActionValue& InputActionValue);
	void DebugToggleSlomo(const FInputActionValue& InputActionValue);
	void DebugCharacterSelect(const FInputActionValue& InputActionValue);
	void DebugToggleSpectatorPawn(const FInputActionValue& InputActionValue);

	void RespawnCharacter();
	void AppearPlayerCharacter();
	void SetSkillCoolTime(const TSubclassOf<AActor>& InSkillClass, const FSkill& InSkillData);
	void SkillCoolTimeDown();
	bool CheckEffectTime(const FEffect& InEffect);
	void AddEffectTimeMap(const FEffect& InEffect);
	void EffectTimeDown();
	void RecoverStamina();
	void DecreaseStamina();
	void SetActorRotationToMouseCursor();
	void RotateMeshToMouseCursor();
	virtual void RotateMeshAxisForward(bool InbMove);
	bool CheckSkillCalibrateRange(const FSkill& InSkillData, AActor*& OutNearActor) const;
	void CalibrateMeshNearActor(const AActor* InNearActor);
	void CalibrateMeshMouseCursor();
	void CalibrateMeshAxisForward();
	void CheckClosestObject();
	void CheckPlayerFalling();
	void ActivateNoiseFX(bool InActivate);
	void StatusSlow(float InSlowValue, bool InbRecovery = false) const;
	void StatusFast(float InFastValue, bool InbRecovery = false) const;
	void StartMouseCursorLinear(bool InbStart, bool InbSustain = true);
	void StartMouseCursorParabola(bool InbStart, bool InbSustain = true, bool InbPathVisible = true);

	UFUNCTION(BlueprintCallable)
	void RestTimerStart();
	
	UFUNCTION(BlueprintCallable)
	void RefreshRestTime();

	UFUNCTION(BlueprintCallable)
	void PlayerJumpBack(AActor* InOccurActor);
	
	UFUNCTION()
	void ResetCharacterData();
	
	UFUNCTION()
	void InteractionAreaBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void InteractionAreaEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	UFUNCTION()
	void NoiseAreaBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void NoiseAreaEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	UFUNCTION(BlueprintCallable)
	void SetDecreaseStaminaTimer(bool InbDecrease);
	
	
private:
	void InitializeCharacterSet();

public:
	UPROPERTY()
	FOnRequestInteractIcon OnRequestInteractIcon;

	UPROPERTY(BlueprintAssignable)
	FInteractionIconVisibility RefreshInteractionVisibility;

	UPROPERTY(BlueprintAssignable)
	FPlayerDeadDone PlayerDeadDone;

	UPROPERTY(BlueprintAssignable)
	FSkillCoolTime RefreshSkillCoolTime;

	UPROPERTY(BlueprintAssignable)
	FEffectTime RefreshEffectTime;

	UPROPERTY(BlueprintAssignable)
	FItemSlotActivate ItemSlotActivate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,Category = PlayerSkill, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> SkillRootScene;

protected:
	FRotator MovingActionForward;
	FRotator MouseCursorRotation;
	bool bInvincible;
	bool bDead;
	float CalibrateYawValue;
	float RestTime;
			
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = PlayerAnimation, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAnimMontage> ActionMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = PlayerAnimation, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAnimSequence> IdleAnimation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = PlayerAnimation, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAnimSequence> RestAnimation;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = PlayerAnimation, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAnimSequence> MoveAnimation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = PlayerAnimation, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAnimSequence> RunAnimation;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = PlayerStatus, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UPlayerStatusComponent> PlayerStatusComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = PlayerStatus, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UPlayerInventoryComponent> PlayerInventoryComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = PlayerStatus, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> AudioListener;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = PlayerAnimation, meta = (AllowPrivateAccess = "true"))
	UPlayerCharacterAnimInstance* PlayerAnimInstance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = PlayerAnimation, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UPoseableMeshComponent> TransformMeshComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Interaction, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USphereComponent> InteractionAreaCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Interaction, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USphereComponent> NoiseAreaCollision;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Interaction, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USphereComponent> SkillCalibrateAreaCollision;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCoreCameraComponent> CoreCameraComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<class UNiagaraComponent> RunFXComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UNiagaraComponent> NoiseFXComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PlayerStatus, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<ACoreSkill> CurrentUsingSkill;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PlayerStatus, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<AActor> InteractableObject;	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input)
	FPlayerActionTypes PlayerActions;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input)
	FPlayerDebugActionTypes PlayerDebugActions;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input)
	FPlayerChordedActionTypes PlayerChordedActions;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input)
	float AxisMoveSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D CharacterMoveAxis;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input, meta = (AllowPrivateAccess = "true"))
	bool bInputAble;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input, meta = (AllowPrivateAccess = "true"))
	bool bUnDetect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input, meta = (AllowPrivateAccess = "true"))
	int CurrentSlotCursor;	
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PlayerStatus, meta = (AllowPrivateAccess = "true"))
	FPlayerCharacter CurrentPlayerData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PlayerStatus, meta = (AllowPrivateAccess = "true"))
	TMap<TSubclassOf<ACoreSkill>, FPlayerSimpleRatio> SkillCoolTimeMap;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PlayerStatus, meta = (AllowPrivateAccess = "true"))
	TMap<EEffectSubTypes, FPlayerEffectValue> EffectTimeMap;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PlayerStatus, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<ACoreSkill> PlayerAtkSkillClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PlayerStatus, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<ACoreSkill> PlayerSkillClass;

	UPROPERTY()
	class ACorePlayerController* CorePlayerController;
	
	UPROPERTY()
	TArray<EEffectSubTypes> ContainEffects;
	
	UPROPERTY()
	FTimerHandle SkillCoolDownTimerHandle;

	UPROPERTY()
	FTimerHandle EffectActiveTimerHandle;
	
	UPROPERTY()
	FTimerHandle ClosestObjectTimerHandle;
	
	UPROPERTY()
	FTimerHandle DecreaseStaminaTimerHandle;
	
	UPROPERTY()
	FTimerHandle RecoverStaminaTimerHandle;
	
	UPROPERTY()
	FTimerHandle MeshRotateCalibrateTimerHandle;
	
	UPROPERTY()
	FTimerHandle FadeTimerHandle;
	
	UPROPERTY()
	FTimerHandle JumpBackTimerHandle;

	UPROPERTY()
	FTimerHandle MouseCursorTimerHandle;

	UPROPERTY()
	FTimerHandle RestTimerHandle;
	
	UPROPERTY()
	UEnhancedInputComponent* CurrentEnhancedInputComponent;

	UPROPERTY(EditDefaultsOnly, Category = "MovingAction")
	bool bMovingAction;
	
	UPROPERTY(EditDefaultsOnly, Category = "MovingAction")
	float MeleeAtkDecreaseSpeedRatio;
	
	UPROPERTY(EditDefaultsOnly, Category = "MovingAction")
	float SkillDecreaseSpeedRatio;

	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	bool bCalibrateSkill;
};
