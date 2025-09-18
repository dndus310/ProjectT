// Fill out your copyright notice in the Description page of Project Settings.


#include "CorePlayerCharacter.h"

#if WITH_EDITOR
#include "Subsystems/AssetEditorSubsystem.h"
#endif

#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Animations/AnimInstance/PlayerCharacterAnimInstance.h"
#include "Kismet/KismetMathLibrary.h"
#include "ProjectT/ProjectT.h"
#include "ProjectT/System/Core/Components/CoreCameraComponent.h"
#include "ProjectT/System/Core/Common/GlobalUtilsLibrary.h"
#include "InputMappingContext.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "Components/CapsuleComponent.h"
#include "Components/PoseableMeshComponent.h"
#include "Components/SphereComponent.h"
#include "ProjectT/Data/Gen/GenerateEnumDataTableKey.h"
#include "ProjectT/Data/Gen/GenerateEnumEffectSubTypes.h"
#include "ProjectT/Data/Gen/GenerateEnumSkillTypes.h"
#include "ProjectT/System/Core/Characters/NPC/CoreNpc.h"
#include "ProjectT/System/Core/Components/PlayerInventoryComponent.h"
#include "ProjectT/System/Core/Components/PlayerStatusComponent.h"
#include "ProjectT/System/Core/GameModes/WorldGameMode.h"
#include "ProjectT/System/Core/Managers/PooledActorManager.h"
#include "ProjectT/System/Core/Objects/LevelObject/CoreInteractionObject.h"
#include "ProjectT/System/Core/Characters/Skill/PyroAtk.h"
#include "ProjectT/System/Core/Common/AssetUtilsLibrary.h"
#include "ProjectT/System/Core/Interfaces/GlobalObjectProvider.h"
#include "ProjectT/System/Core/Managers/CoreCheatManager.h"
#include "ProjectT/System/Core/Managers/DataDam.h"
#include "ProjectT/System/Core/Managers/NotificationCenter.h"
#include "ProjectT/System/Core/Objects/LevelObject/ItemObject.h"
#include "ProjectT/System/Core/Objects/LevelObject/LevelCastObject.h"
#include "ProjectT/System/SinglePlay/Characters/NPC/SPNpcFixed.h"
#include "ProjectT/System/Core/GameModes/CorePlayerController.h"
#include "ProjectT/System/Core/Interfaces/MouseInteraction.h"
#include "ProjectT/System/Core/Mouse/Mouse.h"
#include "ProjectT/System/Core/Widget/CommonHUD.h"
// Temp Only KeyBoard State
#define ONLY_KEYBOARD 1

ACorePlayerCharacter::ACorePlayerCharacter() :
	SkillRootScene(nullptr),
	MovingActionForward(),
	MouseCursorRotation(),
	bInvincible(false),
	bDead(false),
	CalibrateYawValue(0.f),
	RestTime(0.f),
	ActionMontage(nullptr),
	IdleAnimation(nullptr),
	MoveAnimation(nullptr),
	RunAnimation(nullptr),
	PlayerStatusComponent(nullptr),
	PlayerInventoryComponent(nullptr),
	AudioListener(nullptr),
	PlayerAnimInstance(nullptr),
	TransformMeshComponent(nullptr),
	InteractionAreaCollision(nullptr),
	NoiseAreaCollision(nullptr),
	SkillCalibrateAreaCollision(nullptr),
	CameraBoom(nullptr),
	CoreCameraComponent(nullptr),
	DefaultMappingContext(nullptr),
	RunFXComponent(nullptr),
	NoiseFXComponent(nullptr),
	CurrentUsingSkill(nullptr),
	InteractableObject(nullptr),
	AxisMoveSpeed(0.f),
	CharacterMoveAxis(),
	bInputAble(false),
	bUnDetect(false),
	CurrentSlotCursor(-1),
	CorePlayerController(nullptr),
	CurrentEnhancedInputComponent(nullptr),
	bMovingAction(true),
	MeleeAtkDecreaseSpeedRatio(0.f),
	SkillDecreaseSpeedRatio(0.f),
	bCalibrateSkill(true)
{
	PrimaryActorTick.bCanEverTick = true;
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CoreCameraComponent = CreateDefaultSubobject<UCoreCameraComponent>(TEXT("CoreCameraComponent"));
	PlayerStatusComponent = CreateDefaultSubobject<UPlayerStatusComponent>(TEXT("PlayerStatusComponent"));
	PlayerInventoryComponent = CreateDefaultSubobject<UPlayerInventoryComponent>(TEXT("PlayerInventoryComponent"));
	AudioListener = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	TransformMeshComponent = CreateDefaultSubobject<UPoseableMeshComponent>(TEXT("TransformMeshComponent"));
	InteractionAreaCollision = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionAreaComponent"));
	NoiseAreaCollision = CreateDefaultSubobject<USphereComponent>(TEXT("NoiseAreaComponent"));
	SkillCalibrateAreaCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SkillCalibrateAreaComponent"));
	SkillRootScene = CreateDefaultSubobject<USceneComponent>(TEXT("SkillRootScene"));
	RunFXComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("RunFXComponent"));
	NoiseFXComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NoiseFXComponent"));
	NMT_CHECKF(CameraBoom && CoreCameraComponent && PlayerStatusComponent && PlayerInventoryComponent && AudioListener && InteractionAreaCollision && NoiseAreaCollision
				&& SkillCalibrateAreaCollision && SkillRootScene && RunFXComponent && NoiseFXComponent);

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->MaxStepHeight = 0.f;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = (FRotator(0, -1.f, 0));

	GetCapsuleComponent()->SetCollisionProfileName(TEXT("Player"));
	GetCapsuleComponent()->ComponentTags.Add("PlayerCapsule");

	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 500.0f;
	CameraBoom->bDoCollisionTest = false;
	CameraBoom->SetRelativeRotation(FRotator(-55.f, 0, 0));

	CoreCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);

	TransformMeshComponent->SetupAttachment(GetMesh());
	
	InteractionAreaCollision->SetupAttachment(RootComponent);
	InteractionAreaCollision->SetCollisionProfileName(TEXT("InteractionRange"));
	InteractionAreaCollision->OnComponentBeginOverlap.AddDynamic(this, &ACorePlayerCharacter::InteractionAreaBeginOverlap);
	InteractionAreaCollision->OnComponentEndOverlap.AddDynamic(this, &ACorePlayerCharacter::InteractionAreaEndOverlap);

	NoiseAreaCollision->SetupAttachment(RootComponent);
	NoiseAreaCollision->SetCollisionProfileName(TEXT("SoundRange"));
	NoiseAreaCollision->OnComponentBeginOverlap.AddDynamic(this, &ACorePlayerCharacter::NoiseAreaBeginOverlap);
	NoiseAreaCollision->OnComponentEndOverlap.AddDynamic(this, &ACorePlayerCharacter::NoiseAreaEndOverlap);

	SkillCalibrateAreaCollision->SetupAttachment(RootComponent);
	SkillCalibrateAreaCollision->SetCollisionProfileName(TEXT("InteractionRange"));
	SkillCalibrateAreaCollision->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);

	SkillRootScene->SetupAttachment(GetMesh());
	SkillRootScene->SetRelativeLocation(FVector(0.f, 0.f, 50.f));
	
	RunFXComponent->SetupAttachment(GetMesh());
	RunFXComponent->bAutoActivate = false;
	NoiseFXComponent->SetupAttachment(GetMesh());
	NoiseFXComponent->bAutoActivate = false;

	AudioListener->SetupAttachment(RootComponent);
	AudioListener->SetUsingAbsoluteRotation(true);
	AudioListener->SetWorldRotation(GetActorRotation());

	//TODO : Load Relative Path From Table After Added
	if(UNiagaraSystem* LocFindNoiseAsset = Cast<UNiagaraSystem>(StaticLoadObject(UNiagaraSystem::StaticClass(),nullptr,TEXT("/Script/Niagara.NiagaraSystem'/Game/FX/NiagaraSystem/Common/NS_Common_SoundNoiseEffects.NS_Common_SoundNoiseEffects'"))))
		NoiseFXComponent->SetAsset(LocFindNoiseAsset);

	SetupPlayerInputActions();
}

void ACorePlayerCharacter::PostLoad()
{
	Super::PostLoad();

#if WITH_EDITOR
	if(CharacterRowName.IsNone()) return;
	if(!CharacterRowName.ToString().IsNumeric()) return;
	ApplyCharacterConfiguration(CharacterRowName);
#endif
}

void ACorePlayerCharacter::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

#if WITH_EDITOR
	if(!GEditor) return;
	GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->OnAssetOpenedInEditor().AddLambda([this](UObject* Asset, IAssetEditorInstance*)
	{
		if(UBlueprint* Blueprint = Cast<UBlueprint>(Asset))
		{
			if(!Blueprint->GeneratedClass->IsChildOf(ACorePlayerCharacter::StaticClass())) return;

			if(ACorePlayerCharacter* CDO = Cast<ACorePlayerCharacter>(Blueprint->GeneratedClass->GetDefaultObject()))
			{
				CDO->Modify();
				CDO->ApplyCharacterConfiguration(CDO->CharacterRowName);
				if(!CDO->MarkPackageDirty())
					NMT_LOG("Failed to modify ACorePlayerCharacter CDO");
			}
		}
	});
#endif
}

void ACorePlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	InitPlayerCharacter();
}

void ACorePlayerCharacter::InitPlayerCharacter()
{
	InitializeProperty();
	//FIXME : Edit After Appear Animation Added
	//AppearPlayerCharacter();
}

void ACorePlayerCharacter::InitializeProperty()
{
	UWorld* World = UGlobalUtilsLibrary::GetValidWorld(this);
	APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0);
	NMT_CHECKF(PC);

	ULocalPlayer* LocPlayer = Cast<ULocalPlayer>(PC->GetLocalPlayer());
	NMT_CHECKF(LocPlayer);

	UEnhancedInputLocalPlayerSubsystem* InputSystem = LocPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	NMT_CHECKF(InputSystem);

	InputSystem->AddMappingContext(DefaultMappingContext, 0);

	GetMesh()->SetUsingAbsoluteRotation(true);

	CorePlayerController = Cast<ACorePlayerController>(UGameplayStatics::GetPlayerController(World, 0));
	
	PlayerAnimInstance = Cast<UPlayerCharacterAnimInstance>(GetMesh()->GetAnimInstance());
	NMT_CHECKF(PlayerAnimInstance);
	PlayerAnimInstance->SetDefaultProperty(this);

	CameraBoom->bInheritYaw = false;
	CalibrateYawValue = GetActorRotation().Yaw;
	CameraBoom->AddRelativeRotation(FRotator(0, CalibrateYawValue, 0));

	NoiseAreaCollision->SetSphereRadius(PlayerStatusComponent->GetStatusCharMoveSound());
	GetCharacterMovement()->MaxWalkSpeed = PlayerStatusComponent->GetStatusCharMove();
}

void ACorePlayerCharacter::SetSkillClass()
{
	UDataTable* LocSkillDataTable = UAssetUtilsLibrary::LoadDataTableByEnumKey(EDataTableKey::Skill);
	TSubclassOf<AActor> LocSkillClass;
	FPlayerCharacter LocChracterData;
	GetPlayerCharacterData(LocChracterData);

	FString LocSkillRowName = FString::FromInt(LocChracterData.iCharAttackId);
	FSkill* LocSkillDataRow = LocSkillDataTable->FindRow<FSkill>(*LocSkillRowName, TEXT("Not Found FSkill"));
	FString LocTempPath = LocSkillDataRow->sSkillBpPath;
	if(!CHECK_FREE(LocTempPath))
	{
		FString ClassPath = UAssetUtilsLibrary::AssetReferencePathToClassPath(LocTempPath);
		UClass* LoadedClass = StaticLoadClass(UObject::StaticClass(), nullptr, *ClassPath);
		NMT_CHECKF(LoadedClass);

		LocSkillClass = TSubclassOf<AActor>(LoadedClass);
		PlayerAtkSkillClass = LocSkillClass;
	}

	LocSkillRowName = FString::FromInt(LocChracterData.iCharSkillId);
	LocSkillDataRow = LocSkillDataTable->FindRow<FSkill>(*LocSkillRowName, TEXT("Not Found FSkill"));
	LocTempPath = LocSkillDataRow->sSkillBpPath;
	if(!CHECK_FREE(LocTempPath))
	{
		FString ClassPath = UAssetUtilsLibrary::AssetReferencePathToClassPath(LocTempPath);
		UClass* LoadedClass = StaticLoadClass(UObject::StaticClass(), nullptr, *ClassPath);
		NMT_CHECKF(LoadedClass);

		LocSkillClass = TSubclassOf<AActor>(LoadedClass);
		PlayerSkillClass = LocSkillClass;
	}
}

void ACorePlayerCharacter::AxisMoveStarted()
{
	if(MeshRotateCalibrateTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(MeshRotateCalibrateTimerHandle);
		MeshRotateCalibrateTimerHandle.Invalidate();
	}
	UWorld* World = UGlobalUtilsLibrary::GetValidWorld(this);
	if(World->GetTimerManager().IsTimerActive(RestTimerHandle))
		World->GetTimerManager().ClearTimer(RestTimerHandle);
}

void ACorePlayerCharacter::AxisMoveTriggered(const FInputActionValue& Value)
{
	if(!bInputAble) return;
	CharacterMoveAxis = Value.Get<FVector2D>();
	RotateMeshAxisForward(true);
	if(CharacterMoveAxis.Length() == 0)
	{
		ActivateNoiseFX(false);
		ChangeValueOnRun(false);
		PlayerAnimInstance->bIdle = true;
		PlayerAnimInstance->bWalk = false;
		SetDecreaseStaminaTimer(false);
		return;
	}

	PlayerAnimInstance->bIdle = false;
	PlayerAnimInstance->bWalk = true;
	const FVector2D LocNormalizeMoveAxis = UKismetMathLibrary::Normal2D(CharacterMoveAxis);
	if(Controller != nullptr)
	{
		ActivateNoiseFX(true);
		FVector2D LocSelectAxis;
		ChangeValueOnRun(PlayerAnimInstance->bRun);
		CharacterMoveAxis.Length() >= 1.f ? LocSelectAxis = LocNormalizeMoveAxis : LocSelectAxis = CharacterMoveAxis;
		LocSelectAxis = LocSelectAxis.GetRotated(CalibrateYawValue);
		AddMovementInput(FVector(LocSelectAxis.X, LocSelectAxis.Y, 0), 1);
		SetDecreaseStaminaTimer(PlayerAnimInstance->bRun && !PlayerAnimInstance->bAction);
	}
}

void ACorePlayerCharacter::AxisMoveCompleted()
{
	ActivateNoiseFX(false);
	CharacterMoveAxis = FVector2d(0.f, 0.f);
	ChangeValueOnRun(false);
	SetDecreaseStaminaTimer(false);
	RotateMeshAxisForward(false);
	PlayerAnimInstance->bIdle = true;
	PlayerAnimInstance->bWalk = false;
	RestTimerStart();
}

void ACorePlayerCharacter::RunTriggered(const FInputActionValue& Value)
{
	if(PlayerAnimInstance->GetCurrentNonInputState() == ENonInputAnimState::Tired || CurrentUsingSkill)
	{
		PlayerAnimInstance->bRun = false;
	}
	else
	{
		PlayerAnimInstance->bRun = Value.Get<bool>();
	}
}

void ACorePlayerCharacter::RunCompleted()
{
	ChangeValueOnRun(false);
	PlayerAnimInstance->bRun = false;
}

void ACorePlayerCharacter::ChangeValueOnRun(bool InbRun)
{
	if((bMovingAction && PlayerAnimInstance->bAction) || CurrentUsingSkill)
	{
		GetCharacterMovement()->MaxWalkSpeed = PlayerStatusComponent->GetStatusCharMove();
		NoiseAreaCollision->SetSphereRadius(PlayerStatusComponent->GetStatusCharMoveSound());
		RunFXComponent->Deactivate();
		return;
	}
	if(InbRun)
	{
		GetCharacterMovement()->MaxWalkSpeed = PlayerStatusComponent->GetStatusCharRun();
		NoiseAreaCollision->SetSphereRadius(PlayerStatusComponent->GetStatusCharRunSound());
		RunFXComponent->Activate(false);
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = PlayerStatusComponent->GetStatusCharMove();
		NoiseAreaCollision->SetSphereRadius(PlayerStatusComponent->GetStatusCharMoveSound());
		RunFXComponent->Deactivate();
	}
}

void ACorePlayerCharacter::ActivateMeleeAttack()
{
	if(CurrentUsingSkill)
	{
		if(CurrentUsingSkill.GetClass() == PlayerAtkSkillClass || !bInputAble) return;
		FSkill LocSkillData;
		CurrentUsingSkill->GetSkillData(LocSkillData);
		if(CurrentUsingSkill.GetClass() == PlayerSkillClass)
		{
			if(!CurrentUsingSkill->GetActivated()) SetSkillState(EPlayerSkillState::Activate, LocSkillData);	
		}
		else
		{
			SetItemSkillState(EPlayerSkillState::Activate, LocSkillData);
		}
		return;
	}
	if(!bInputAble || SkillCoolTimeMap.Contains(PlayerAtkSkillClass)) return;
	UWorld* World = UGlobalUtilsLibrary::GetValidWorld(this);
	AWorldGameMode* LocGameMode = UGlobalUtilsLibrary::GetGameModeChecked<AWorldGameMode>(World);
	if(UPooledActorManager* LocPoolManager = IWorldObjectProvider::Execute_GetPooledActorManager(LocGameMode))
	{
		CurrentUsingSkill = Cast<ACoreSkill>(LocPoolManager->Obtain(PlayerAtkSkillClass));
		CurrentUsingSkill->SetSkillOwner(this);
		FSkill LocMeleeAtkSkillData;
		CurrentUsingSkill->GetSkillData(LocMeleeAtkSkillData); 
		SetMeleeAtkSkillState(EPlayerSkillState::Start,LocMeleeAtkSkillData);
	}
	SetDecreaseStaminaTimer(false);
	ChangeValueOnRun(false);
}

void ACorePlayerCharacter::ActivateSkill()
{
	if(CurrentUsingSkill)
	{
		if(PlayerSkillClass == CurrentUsingSkill->GetClass() && !SkillCoolTimeMap.Contains(PlayerSkillClass))
		{
			FSkill LocSkillData;
			CurrentUsingSkill->GetSkillData(LocSkillData);
			SetSkillState(EPlayerSkillState::Release, LocSkillData);
		}
		return;
	}
	if(!bInputAble || SkillCoolTimeMap.Contains(PlayerSkillClass)) return;
	UWorld* World = UGlobalUtilsLibrary::GetValidWorld(this);
	AWorldGameMode* LocGameMode = UGlobalUtilsLibrary::GetGameModeChecked<AWorldGameMode>(World);
	if(UPooledActorManager* LocPoolManager = IWorldObjectProvider::Execute_GetPooledActorManager(LocGameMode))
	{
		CurrentUsingSkill = Cast<ACoreSkill>(LocPoolManager->Obtain(PlayerSkillClass));
		CurrentUsingSkill->SetSkillOwner(this);
		FSkill LocSkillData;
		CurrentUsingSkill->GetSkillData(LocSkillData);
		SetSkillState(EPlayerSkillState::Start, LocSkillData);
	}
	SetDecreaseStaminaTimer(false);
	ChangeValueOnRun(false);
}

void ACorePlayerCharacter::ActivateItemSkill(int InCurrentInput, int InItemSkillId)
{
	FString LocItemSkillRow = FString::FromInt(InItemSkillId);
	UDataTable* LocDataTable = UAssetUtilsLibrary::LoadDataTableByEnumKey(EDataTableKey::Skill);
	FSkill* LocSkillDataRow = LocDataTable->FindRow<FSkill>(*LocItemSkillRow, TEXT("Not Found FSkill"));
	if(!LocSkillDataRow) return;

	FString ClassPath = UAssetUtilsLibrary::AssetReferencePathToClassPath(LocSkillDataRow->sSkillBpPath);
	UClass* LoadedClass = StaticLoadClass(UObject::StaticClass(), nullptr, *ClassPath);
	if(!NMT_MSG_ENSURE(LoadedClass, "Skill Class Not Found : %s", *ClassPath)) return;

	TSubclassOf<AActor> LocItemClass = TSubclassOf<AActor>(LoadedClass);
	if(CurrentUsingSkill)
	{
		if(InCurrentInput != CurrentSlotCursor) return;
		if(LocItemClass == CurrentUsingSkill->GetClass())
		{
			SetItemSkillState(EPlayerSkillState::Release);
			if(ItemSlotActivate.IsBound()) ItemSlotActivate.Broadcast(CurrentSlotCursor, false);
			CurrentSlotCursor = -1;			
		}
		return;
	}
	if(PlayerAnimInstance->bAction) return;
	UWorld* World = UGlobalUtilsLibrary::GetValidWorld(this);
	AWorldGameMode* LocGameMode = UGlobalUtilsLibrary::GetGameModeChecked<AWorldGameMode>(World);
	if(UPooledActorManager* LocPoolManager = IWorldObjectProvider::Execute_GetPooledActorManager(LocGameMode))
	{
		CurrentUsingSkill = Cast<ACoreSkill>(LocPoolManager->Obtain(LocItemClass));
		CurrentUsingSkill->SetSkillOwner(this);
		FSkill LocSkillData;
		CurrentUsingSkill->GetSkillData(LocSkillData);
		CurrentSlotCursor = InCurrentInput;
		if(ItemSlotActivate.IsBound()) ItemSlotActivate.Broadcast(CurrentSlotCursor, true);
		ESkillTypes LocItemSkillType = UGlobalUtilsLibrary::GetStringToEnum<ESkillTypes>(LocSkillData.sSkillType);
		LocItemSkillType == ESkillTypes::Buff? SetItemSkillState(EPlayerSkillState::Activate, LocSkillData) : SetItemSkillState(EPlayerSkillState::Start, LocSkillData); 
	}
	SetDecreaseStaminaTimer(false);
}

void ACorePlayerCharacter::SetInputable(bool InbEnable)
{
	bInputAble = InbEnable;
}

void ACorePlayerCharacter::SetInvincible(bool InbInvincible)
{
	bInvincible = InbInvincible;
}

void ACorePlayerCharacter::StartCountDown_Implementation()
{
	NOT_IMPLEMENTED()
}

void ACorePlayerCharacter::PlayActionMontage(FName InSectionName, FOnMontageEnded InEndedFunction, FOnMontageBlendingOutStarted InBlendOutFunction) const
{
	if(!ActionMontage) return;
	PlayerAnimInstance->Montage_Play(ActionMontage, 1, EMontagePlayReturnType::MontageLength, 0, true);
	FOnMontageEnded LocDefaultEndedDelegate;
	FOnMontageBlendingOutStarted LocDefaultBlendingOutDelegate;
	PlayerAnimInstance->Montage_SetEndDelegate(LocDefaultEndedDelegate, ActionMontage);
    PlayerAnimInstance->Montage_SetBlendingOutDelegate(LocDefaultBlendingOutDelegate, ActionMontage);
	PlayerAnimInstance->Montage_SetEndDelegate(InEndedFunction,ActionMontage);
	PlayerAnimInstance->Montage_SetBlendingOutDelegate(InBlendOutFunction,ActionMontage);
	PlayerAnimInstance->Montage_JumpToSection(InSectionName);
	PlayerAnimInstance->bAction = true;
}

void ACorePlayerCharacter::PlayCommonMontage(FName InSectionName, FOnMontageEnded InEndedFunction, FOnMontageBlendingOutStarted InBlendOutFunction) const
{
	UAnimMontage* LocCommonMontage;
	PlayerAnimInstance->GetCommonMontage(LocCommonMontage);
	if(LocCommonMontage)
	{
		FOnMontageEnded LocDefaultEndedDelegate;
		FOnMontageBlendingOutStarted LocDefaultBlendingOutDelegate;
		PlayerAnimInstance->Montage_Play(LocCommonMontage, 1, EMontagePlayReturnType::MontageLength, 0, true);
		PlayerAnimInstance->Montage_SetEndDelegate(LocDefaultEndedDelegate, LocCommonMontage);
		PlayerAnimInstance->Montage_SetBlendingOutDelegate(LocDefaultBlendingOutDelegate, LocCommonMontage);

		PlayerAnimInstance->Montage_SetEndDelegate(InEndedFunction);
		PlayerAnimInstance->Montage_SetBlendingOutDelegate(InBlendOutFunction);
		PlayerAnimInstance->Montage_JumpToSection(InSectionName);
		PlayerAnimInstance->bAction = true;
	}
}

void ACorePlayerCharacter::StopActionMontage() const
{
	if(!ActionMontage) return;
	if(PlayerAnimInstance->GetInstanceForMontage(ActionMontage))
	{
		FAnimMontageInstance* LocMontageInstance = PlayerAnimInstance->GetInstanceForMontage(ActionMontage);
		if(LocMontageInstance->OnMontageEnded.IsBound())
		{
			LocMontageInstance->OnMontageEnded.Unbind();
		}		
	}
	PlayerAnimInstance->Montage_Stop(0.f, ActionMontage);
	PlayerAnimInstance->bAction = false;
}

void ACorePlayerCharacter::SetCurrentSkill(bool InbSet, ACoreSkill* InSetSkill)
{
	InbSet ? CurrentUsingSkill = InSetSkill : CurrentUsingSkill = nullptr;
}

void ACorePlayerCharacter::GetSkillClass(TSubclassOf<AActor>& OutAtkSkillClass, TSubclassOf<AActor>& OutSkillClass) const
{
	OutAtkSkillClass = PlayerAtkSkillClass;
	OutSkillClass = PlayerSkillClass;
}

void ACorePlayerCharacter::GetPlayerCharacterData(FPlayerCharacter& OutPlayerCharacterData) const
{
	OutPlayerCharacterData = PlayerStatusComponent->GetPreviousPlayerStatus();

	TArray<AActor*> Actors;
}


void ACorePlayerCharacter::InteractionStarted()
{
	if(InteractableObject != nullptr)
	{
		SetInputable(false);
		PlayerAnimInstance->bIdle = true;
		PlayerAnimInstance->bWalk = false;
		SetDecreaseStaminaTimer(false);
		
		EInteractionObjectType LocInteractionType = IObjectInteractionControl::Execute_GetInteractionObjectType(InteractableObject);
		IObjectInteractionControl* LocInterface = Cast<IObjectInteractionControl>(InteractableObject);
		switch(LocInteractionType)
		{
			case EInteractionObjectType::Item:
			{
				LocInterface->TryInteraction(FInteractionData(),FOnInteractionSuccess::CreateLambda([this](const EInteractionResultType InResultType)
				{
					if(InResultType == EInteractionResultType::SuccessComplete)
					{
						if(PlayerInventoryComponent->TryAddData(InteractableObject))
						{
							IItemObjectControl::Execute_ObtainItem(InteractableObject);
						}						
						InteractionCompleted();
					}						
				}));
				break;
			}
			case EInteractionObjectType::LevelObject:
			{
				FInteractionData LocCurData;
				TArray<FName> LocReserveRowNames;
				PlayerInventoryComponent->GetReserveItemRowNames(LocReserveRowNames);
				if(LocReserveRowNames.Num() <= 0)
				{
					InteractionCompleted();
					return;
				}
				LocCurData.RowNames = LocReserveRowNames;
				LocInterface->TryInteraction(LocCurData, FOnInteractionSuccess::CreateLambda([this](const EInteractionResultType InResultType)
				{
					switch(InResultType)
					{
						case EInteractionResultType::Fail:
						{
							InteractionCompleted();
							break;
						}
						case EInteractionResultType::SuccessPre:
						{
							//TODO : GetCastId From Interface If Create Other Class(Type) 
							ALevelCastObject* LocTest = Cast<ALevelCastObject>(InteractableObject);
							int LocMatchSlot;
							PlayerInventoryComponent->GetMatchSlotNumber(*FString::FromInt(LocTest->GetCastItemID()),LocMatchSlot);
							if(LocMatchSlot != -1) PlayerInventoryComponent->UseGimmickSlotItem(LocMatchSlot);
							InteractionCompleted();
							break;
						}
						case EInteractionResultType::SuccessUpdate: break;
						case EInteractionResultType::SuccessComplete: break;
						default: NMT_LOG("Result Type None"); break;
					}
				}));
				break;
			}
			case EInteractionObjectType::ObjectiveCharacter:
			{
				LocInterface->TryInteraction(FInteractionData(), FOnInteractionSuccess::CreateLambda([this](const EInteractionResultType InResultType)
				{
					switch(InResultType)
					{
						case EInteractionResultType::SuccessPre:
						{
							InteractionCompleted();
							break;
						}
						case EInteractionResultType::SuccessComplete:
						{
							//TODO : Complete Action Bind
							SetInputable(true);
							break;
						}
						default: NMT_LOG("Result Type None") break;
						}
					}));
				break;
			}
			default: NMT_LOG("Interaction Type None");
		}				
	}
}

void ACorePlayerCharacter::InteractionCompleted()
{
	SetInputable(true);
	if(InteractableObject != nullptr)
	{		
		IVisualizable::Execute_HideVisual(InteractableObject);
	}
	InteractableObject = nullptr;
}

void ACorePlayerCharacter::UseItemSlot1()
{
	int LocItemSkillId;
	PlayerInventoryComponent->UseSpendSlotItem(0, LocItemSkillId);
	if(LocItemSkillId == -1) return;
	ActivateItemSkill(0, LocItemSkillId);
}

void ACorePlayerCharacter::UseItemSlot2()
{
	int LocItemSkillId;
	PlayerInventoryComponent->UseSpendSlotItem(1, LocItemSkillId);
	if(LocItemSkillId == -1) return;
	ActivateItemSkill(1, LocItemSkillId);
}

void ACorePlayerCharacter::UseItemSlot3()
{
	int LocItemSkillId;
	PlayerInventoryComponent->UseSpendSlotItem(2, LocItemSkillId);
	if(LocItemSkillId == -1) return;
	ActivateItemSkill(2, LocItemSkillId);
}

void ACorePlayerCharacter::ToggleGameMenu()
{
	ACorePlayerController* CPC = Cast<ACorePlayerController>(CorePlayerController);
	NMT_CHECKF(CPC);
	UCommonHUD* CHUD = Cast<UCommonHUD>(CPC->GetCurrentCommonHUD());
	NMT_CHECKF(CHUD);
	CHUD->ToggleGameMenuEvent();	
}

void ACorePlayerCharacter::DebugToggleCheat(const FInputActionValue& InputActionValue)
{
	UCoreCheatManager* CCM = UGlobalUtilsLibrary::GetSharedCoreCheatManager(this);
	CCM->DebugToggleCheat();
}

void ACorePlayerCharacter::DebugToggleInvincible(const FInputActionValue& InputActionValue)
{
	UCoreCheatManager* CCM = UGlobalUtilsLibrary::GetSharedCoreCheatManager(this);
	CCM->DebugTogglePlayerInvincibleHP();
}

void ACorePlayerCharacter::DebugTogglePlayerDamage(const FInputActionValue& InputActionValue)
{
	UCoreCheatManager* CCM = UGlobalUtilsLibrary::GetSharedCoreCheatManager(this);
	CCM->DebugTogglePlayerDamage();
}

void ACorePlayerCharacter::DebugTogglePlayerSpeed(const FInputActionValue& InputActionValue)
{
	UCoreCheatManager* CCM = UGlobalUtilsLibrary::GetSharedCoreCheatManager(this);
	CCM->DebugTogglePlayerSpeed();
}

void ACorePlayerCharacter::DebugToggleFrustum(const FInputActionValue& InputActionValue)
{
	UCoreCheatManager* CCM = UGlobalUtilsLibrary::GetSharedCoreCheatManager(this);
	CCM->DebugToggleVision();
}

void ACorePlayerCharacter::DebugToggleHitCollision(const FInputActionValue& InputActionValue)
{
	UCoreCheatManager* CCM = UGlobalUtilsLibrary::GetSharedCoreCheatManager(this);
	CCM->DebugToggleHitCollision();
}

void ACorePlayerCharacter::DebugToggleTimeStop(const FInputActionValue& InputActionValue)
{
	UCoreCheatManager* CCM = UGlobalUtilsLibrary::GetSharedCoreCheatManager(this);
	CCM->DebugToggleTimeStop();
}

void ACorePlayerCharacter::DebugToggleSlomo(const FInputActionValue& InputActionValue)
{
	UCoreCheatManager* CCM = UGlobalUtilsLibrary::GetSharedCoreCheatManager(this);
	CCM->DebugToggleSlomo();
}

void ACorePlayerCharacter::DebugCharacterSelect(const FInputActionValue& InputActionValue)
{
	UCoreCheatManager* CCM = UGlobalUtilsLibrary::GetSharedCoreCheatManager(this);
	CCM->DebugToggleSlomo();
}

void ACorePlayerCharacter::DebugToggleSpectatorPawn(const FInputActionValue& InputActionValue)
{
	UCoreCheatManager* CCM = UGlobalUtilsLibrary::GetSharedCoreCheatManager(this);
	CCM->DebugSpectatorPlay();
}

void ACorePlayerCharacter::RespawnCharacter()
{
	//FIXME : camera fade and finish function Bind at gamemode
	// CoreCameraComponent->StartFade(EFadeType::FadeOut,FOnStaticFinishCameraFade::CreateLambda([&]
	// {
	// 	if(PlayerDeadDone.IsBound()) PlayerDeadDone.Broadcast();
	// 	ResetCharacterData();
	// }),1.5f,0.1f);
}

void ACorePlayerCharacter::ResetCharacterData()
{
	UWorld* World = UGlobalUtilsLibrary::GetValidWorld(this);
	AWorldGameMode* LocGameMode = UGlobalUtilsLibrary::GetGameModeChecked<AWorldGameMode>(World);
	TArray<AActor*> LocSpawnPoints;
	LocGameMode->GetPlayerSpawnPoints(LocSpawnPoints);
	int LocSelectIndex = FMath::RandRange(0, LocSpawnPoints.Num() - 1);
	SetActorLocation(LocSpawnPoints[LocSelectIndex]->GetActorLocation());
	PlayerStatusComponent->ResetStatusDefault();
	UNotificationCenter* LocNotiCenter = IGlobalObjectProvider::Execute_GetNotificationCenter(UGameplayStatics::GetGameInstance(this));
	LocNotiCenter->Post(ENotificationTypes::GameState_Restarted);
	AppearPlayerCharacter();
}

void ACorePlayerCharacter::AppearPlayerCharacter()
{
	if(bDead)
	{
		bDead = false;
	}
	PlayerAnimInstance->SetNonInputAnimState(ENonInputAnimState::GetUp);
}

void ACorePlayerCharacter::SetSkillCoolTime(const TSubclassOf<AActor>& InSkillClass, const FSkill& InSkillData)
{
	UWorld* World = UGlobalUtilsLibrary::GetValidWorld(this);
	UCoreCheatManager* CCM = UGlobalUtilsLibrary::GetSharedCoreCheatManager(this);
	if(CCM->GetDebugStatus().bDebugPlayerInvincibleSkill)
	{
		if(World->GetTimerManager().IsTimerActive(SkillCoolDownTimerHandle))
			World->GetTimerManager().ClearTimer(SkillCoolDownTimerHandle);

		SkillCoolTimeMap.Empty();
		return;
	}
	
	if(!TSubclassOf<ACoreSkill>(InSkillClass)) return;
	TSubclassOf<ACoreSkill> LocCastClass = TSubclassOf<ACoreSkill>(InSkillClass);
	FPlayerSimpleRatio LocSkillCoolTimeRatio;
	LocSkillCoolTimeRatio.CurrentValue = InSkillData.fCooldown;
	LocSkillCoolTimeRatio.MaxValue = InSkillData.fCooldown;
	SkillCoolTimeMap.Add(LocCastClass, LocSkillCoolTimeRatio);
	
	if(!World->GetTimerManager().IsTimerActive(SkillCoolDownTimerHandle))
		World->GetTimerManager().SetTimer(SkillCoolDownTimerHandle, this, &ACorePlayerCharacter::SkillCoolTimeDown, World->DeltaTimeSeconds, true);
}

void ACorePlayerCharacter::SetUnDetectState(bool InbUnDetect)
{
	bUnDetect = InbUnDetect;
}

void ACorePlayerCharacter::SetSlotCursor(int InSetSlotCursor)
{
	CurrentSlotCursor = InSetSlotCursor;
}

void ACorePlayerCharacter::GetAudioListener(USceneComponent*& OutAudioListener)
{
	OutAudioListener = AudioListener;
}

void ACorePlayerCharacter::SkillCoolTimeDown()
{
	TArray<TSubclassOf<ACoreSkill>> LocSkillClass;
	SkillCoolTimeMap.GetKeys(LocSkillClass);
	for(TSubclassOf<ACoreSkill> LocKey : LocSkillClass)
	{
		float LocCurRemainTime = SkillCoolTimeMap.Find(LocKey)->CurrentValue;
		LocCurRemainTime = LocCurRemainTime - GetWorld()->DeltaTimeSeconds;
		if(LocCurRemainTime <= 0.f)
		{
			SkillCoolTimeMap.Remove(LocKey);
			if(SkillCoolTimeMap.IsEmpty())
			{
				GetWorld()->GetTimerManager().ClearTimer(SkillCoolDownTimerHandle);
				SkillCoolDownTimerHandle.Invalidate();
				if(RefreshSkillCoolTime.IsBound()) RefreshSkillCoolTime.Broadcast(LocKey, 0.f, true);
			}
		}
		else
		{
			SkillCoolTimeMap.Find(LocKey)->CurrentValue = LocCurRemainTime;
			float LocCurRemainRatio = SkillCoolTimeMap.Find(LocKey)->CurrentValue / SkillCoolTimeMap.Find(LocKey)->MaxValue; 
			if(RefreshSkillCoolTime.IsBound()) RefreshSkillCoolTime.Broadcast(LocKey, LocCurRemainRatio, false);						
		}		
	}
}

bool ACorePlayerCharacter::CheckEffectTime(const FEffect& InEffect)
{
	bool LocbRefreshEffect = false;
	EEffectSubTypes LocEffectSubType = UGlobalUtilsLibrary::GetStringToEnum<EEffectSubTypes>(InEffect.sEffect);
	if(EffectTimeMap.Find(LocEffectSubType))
	{
		if(EffectTimeMap.Find(LocEffectSubType)->EffectValue > InEffect.fEffectValue) return LocbRefreshEffect;
		if(EffectTimeMap.Find(LocEffectSubType)->EffectValue == InEffect.fEffectValue)
		{
			if(EffectTimeMap.Find(LocEffectSubType)->RemainTime < InEffect.fEffectDuration) LocbRefreshEffect = true;
		}
		else LocbRefreshEffect = true;
	}
	else LocbRefreshEffect = true;
	return LocbRefreshEffect;
}

void ACorePlayerCharacter::AddEffectTimeMap(const FEffect& InEffect)
{
	FPlayerEffectValue LocEffectValue;
	EEffectSubTypes LocEffectSubType = UGlobalUtilsLibrary::GetStringToEnum<EEffectSubTypes>(InEffect.sEffect);
	if(EffectTimeMap.Find(LocEffectSubType))
	{
		if(EffectTimeMap.Find(LocEffectSubType)->EffectValue > InEffect.fEffectValue) return;
		if(EffectTimeMap.Find(LocEffectSubType)->EffectValue == InEffect.fEffectValue)
		{
			if(EffectTimeMap.Find(LocEffectSubType)->RemainTime < InEffect.fEffectDuration)
			{
				LocEffectValue.RemainTime = InEffect.fEffectDuration;
				LocEffectValue.MaxTime = InEffect.fEffectDuration;
				LocEffectValue.EffectValue = InEffect.fEffectValue;
				EffectTimeMap.Add(LocEffectSubType, LocEffectValue);
			}	
		}
		else
		{
			LocEffectValue.RemainTime = InEffect.fEffectDuration;
			LocEffectValue.MaxTime = InEffect.fEffectDuration;
			LocEffectValue.EffectValue = InEffect.fEffectValue;
			EffectTimeMap.Add(LocEffectSubType, LocEffectValue);
		}
	}
	else
	{
		LocEffectValue.RemainTime = InEffect.fEffectDuration;
		LocEffectValue.MaxTime = InEffect.fEffectDuration;
		LocEffectValue.EffectValue = InEffect.fEffectValue;
		EffectTimeMap.Add(LocEffectSubType, LocEffectValue);
	}
	if(!GetWorld()->GetTimerManager().IsTimerActive(EffectActiveTimerHandle))
		GetWorld()->GetTimerManager().SetTimer(EffectActiveTimerHandle, this, &ACorePlayerCharacter::EffectTimeDown, GetWorld()->DeltaTimeSeconds, true);
}

void ACorePlayerCharacter::EffectTimeDown()
{
	TArray<EEffectSubTypes> LocEffectSubType;
	EffectTimeMap.GetKeys(LocEffectSubType);
	FEffect LocRemoveEffect;
	for(EEffectSubTypes LocKey : LocEffectSubType)
	{
		float LocCurRemainTime = EffectTimeMap.Find(LocKey)->RemainTime;
		LocCurRemainTime = LocCurRemainTime - GetWorld()->DeltaTimeSeconds;
		if(LocCurRemainTime <= 0.f)
		{
			TArray<FEffect> LocRemoveEffects;
			LocRemoveEffect.sEffect = EnumToString(LocKey);
			LocRemoveEffect.fEffectValue = EffectTimeMap.Find(LocKey)->EffectValue;
			LocRemoveEffects.Add(LocRemoveEffect);
			ApplyRecoveryEvent_Implementation(LocRemoveEffects);
		}
		else
		{
			EffectTimeMap.Find(LocKey)->RemainTime = LocCurRemainTime; 
			float LocCurRemainRatio = LocCurRemainTime / EffectTimeMap.Find(LocKey)->MaxTime;
			if(RefreshEffectTime.IsBound()) RefreshEffectTime.Broadcast(LocKey, LocCurRemainRatio, false);						
		}		
	}	
}

void ACorePlayerCharacter::PlayerJumpBack(AActor* InOccurActor)
{
	//TODO : Velocity From Ouucr Actor Edit
	SetDecreaseStaminaTimer(false);
	PlayerAnimInstance->SetNonInputAnimState(ENonInputAnimState::JumpBackStart);
	float LocPower = 100.f;
	FVector LocBackVector = LocPower * (GetActorForwardVector() * -1.f);
	FVector LocLaunchVelocity;
	UGlobalUtilsLibrary::SuggestProjectileVelocityAtLocation(GetWorld(), LocLaunchVelocity, GetActorLocation(), GetActorLocation() + LocBackVector, 30.f, 0.f, 1.f);
	LaunchCharacter(LocLaunchVelocity, true, true);
	if(!GetWorld()->GetTimerManager().IsTimerActive(JumpBackTimerHandle))
		GetWorld()->GetTimerManager().SetTimer(JumpBackTimerHandle, this, &ACorePlayerCharacter::CheckPlayerFalling, GetWorld()->DeltaTimeSeconds, true);
}

void ACorePlayerCharacter::CheckPlayerFalling()
{
	if(!GetMovementComponent()->IsFalling())
	{
		GetWorld()->GetTimerManager().ClearTimer(JumpBackTimerHandle);
		JumpBackTimerHandle.Invalidate();
		UPlayerCharacterAnimInstance* LocAnimInstance;
		GetAnimInstance(LocAnimInstance);
		LocAnimInstance->SetNonInputAnimState(ENonInputAnimState::JumpBackEnd);
	}
}

void ACorePlayerCharacter::ActivateNoiseFX(bool InActivate)
{
	if(InActivate)
	{
		if(!NoiseFXComponent->IsActive()) NoiseFXComponent->Activate(true);
		float LocFXRadius = PlayerAnimInstance->bRun ? LocFXRadius = PlayerStatusComponent->GetStatusCharRunSound() * 2.f
													: LocFXRadius = PlayerStatusComponent->GetStatusCharMoveSound() * 2.f;
		NoiseFXComponent->SetFloatParameter(TEXT("Wave Radius"), LocFXRadius);
	}
	else
	{
		NoiseFXComponent->Deactivate();
	}	
}

void ACorePlayerCharacter::StatusSlow(float InSlowValue, bool InbRecovery) const
{
	float LocBeforeValue = EffectTimeMap.Find(EEffectSubTypes::Slow)? LocBeforeValue = EffectTimeMap.Find(EEffectSubTypes::Slow)->EffectValue : LocBeforeValue = 0.f;  
	if(LocBeforeValue == InSlowValue) return;
	if(!InbRecovery) InSlowValue = InSlowValue * -1.f;
	float LocSlowValue = InSlowValue - LocBeforeValue;
	PlayerStatusComponent->AddStatusCharMove(LocSlowValue);
	PlayerStatusComponent->AddStatusCharRun(LocSlowValue);
}

void ACorePlayerCharacter::StatusFast(float InFastValue, bool InbRecovery) const
{
	float LocBeforeValue = EffectTimeMap.Find(EEffectSubTypes::Fast)? LocBeforeValue = EffectTimeMap.Find(EEffectSubTypes::Fast)->EffectValue : LocBeforeValue = 0.f;  
	if(LocBeforeValue == InFastValue) return;
	if(InbRecovery) InFastValue = InFastValue * -1.f;
	float LocFastValue = InFastValue - LocBeforeValue;
	PlayerStatusComponent->AddStatusCharMove(LocFastValue);
	PlayerStatusComponent->AddStatusCharRun(LocFastValue);
}

void ACorePlayerCharacter::StartMouseCursorLinear(const bool InbStart, bool InbSustain)
{
	if(InbStart)
	{
		FSkill LocCurrentSkillData;
		CurrentUsingSkill->GetSkillData(LocCurrentSkillData);
		float LocRange = LocCurrentSkillData.fSkillRange != 0.f ? LocRange = LocCurrentSkillData.fSkillRange : LocRange = 0;
		float LocAffectRange = LocCurrentSkillData.fEffectRange != 0.f ? LocAffectRange = LocCurrentSkillData.fEffectRange : LocAffectRange = 0;
		TArray<AActor*> LocAffectActors;
		LocAffectActors.Add(CurrentUsingSkill);
		if(InbSustain)
		{
			CorePlayerController->MouseAimTracing(true, this, LocAffectActors, LocRange, LocAffectRange, EMouseResultType::Linear);
		}
		else
		{
			FHitResult LocHitResult = CorePlayerController->GetMouseCursorHitResult(this, LocAffectActors, LocRange, EMouseResultType::Linear);
			IMouseInteraction::Execute_RefreshCursorHitResult(CurrentUsingSkill,LocHitResult);
		}
	}
	else
	{
		TArray<AActor*> LocAffectActors;
		CorePlayerController->MouseAimTracing(false, nullptr, LocAffectActors, 0.f, 0.f, EMouseResultType::None);
	}
}

void ACorePlayerCharacter::StartMouseCursorParabola(const bool InbStart, bool InbSustain, bool InbPathVisible)
{
	if(InbStart)
	{
		FSkill LocCurrentSkillData;
		CurrentUsingSkill->GetSkillData(LocCurrentSkillData);
		float LocRange = LocCurrentSkillData.fSkillRange != 0.f ? LocRange = LocCurrentSkillData.fSkillRange : LocRange = 0;
		float LocAffectRange = LocCurrentSkillData.fEffectRange != 0.f ? LocAffectRange = LocCurrentSkillData.fEffectRange : LocAffectRange = 0;
		TArray<AActor*> LocAffectActors;
		LocAffectActors.Add(CurrentUsingSkill);
		if(InbSustain)
		{
			CorePlayerController->MouseAimTracing(true, this, LocAffectActors, LocRange, LocAffectRange, EMouseResultType::Parabola, InbPathVisible);
		}
		else
		{
			FHitResult LocHitResult = CorePlayerController->GetMouseCursorHitResult(this, LocAffectActors, LocRange, EMouseResultType::Parabola);
			IMouseInteraction::Execute_RefreshCursorHitResult(CurrentUsingSkill,LocHitResult);
		}					
	}
	else
	{
		TArray<AActor*> LocAffectActors;
		CorePlayerController->MouseAimTracing(false, nullptr, LocAffectActors, 0.f, 0.f, EMouseResultType::None);
	}
}

void ACorePlayerCharacter::RestTimerStart()
{
	RestTime = 0.f;
	UWorld* World = UGlobalUtilsLibrary::GetValidWorld(this);
	if(!World->GetTimerManager().IsTimerActive(RestTimerHandle))
		World->GetTimerManager().SetTimer(RestTimerHandle, this, &ACorePlayerCharacter::RefreshRestTime, World->DeltaTimeSeconds, true);
}

void ACorePlayerCharacter::RefreshRestTime()
{
	if(RestTime > 3.f)
	{
		UWorld* World = UGlobalUtilsLibrary::GetValidWorld(this);
		if(World)
			World->GetTimerManager().ClearTimer(RestTimerHandle);
		PlayerAnimInstance->bRest = true;
	}
	else
	{
		UWorld* World = UGlobalUtilsLibrary::GetValidWorld(this);
		RestTime = RestTime + World->DeltaTimeSeconds;
	}
	
}

void ACorePlayerCharacter::SetDecreaseStaminaTimer(bool InbDecrease)
{
	if(InbDecrease)
	{
		if(GetWorld()->GetTimerManager().IsTimerActive(RecoverStaminaTimerHandle))
		{
			GetWorld()->GetTimerManager().ClearTimer(RecoverStaminaTimerHandle);
			RecoverStaminaTimerHandle.Invalidate();
		}
		if(!GetWorld()->GetTimerManager().IsTimerActive(DecreaseStaminaTimerHandle))
			GetWorld()->GetTimerManager().SetTimer(DecreaseStaminaTimerHandle, this, &ACorePlayerCharacter::DecreaseStamina, GetWorld()->DeltaTimeSeconds, true);
	}
	else
	{
		if(GetWorld()->GetTimerManager().IsTimerActive(DecreaseStaminaTimerHandle))
		{
			GetWorld()->GetTimerManager().ClearTimer(DecreaseStaminaTimerHandle);
			DecreaseStaminaTimerHandle.Invalidate();
		}
		if(!GetWorld()->GetTimerManager().IsTimerActive(RecoverStaminaTimerHandle))
			GetWorld()->GetTimerManager().SetTimer(RecoverStaminaTimerHandle, this, &ACorePlayerCharacter::RecoverStamina, GetWorld()->DeltaTimeSeconds, true);
	}
}

void ACorePlayerCharacter::DecreaseStamina()
{
	if(PlayerStatusComponent->GetStatusStamina() <= 0.f)
	{
		bInputAble = false;
		GetWorld()->GetTimerManager().ClearTimer(DecreaseStaminaTimerHandle);
		DecreaseStaminaTimerHandle.Invalidate();
		PlayerAnimInstance->SetNonInputAnimState(ENonInputAnimState::Tired);
		SetDecreaseStaminaTimer(false);
		ChangeValueOnRun(false);
	}
	else
	{
		PlayerStatusComponent->SpendPlayerStamina(true);
	}
}

void ACorePlayerCharacter::SetActorRotationToMouseCursor()
{
	APlayerController* LocController = UGameplayStatics::GetPlayerController(GetWorld(),0);
	if(!LocController) return;
	FHitResult LocCursorHitResult;
	LocController->GetHitResultUnderCursorByChannel(TraceTypeQuery3, true, LocCursorHitResult);
	if(LocCursorHitResult.bBlockingHit)
	{
		FRotator LocToCursorRotator(0.f, UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), LocCursorHitResult.Location).Yaw, 0.f);
		SetActorRotation(LocToCursorRotator);
	}
}

void ACorePlayerCharacter::RotateMeshToMouseCursor()
{
	APlayerController* LocController = UGameplayStatics::GetPlayerController(GetWorld(),0);
	if(!LocController) return;
	GetWorld()->GetTimerManager().ClearTimer(MeshRotateCalibrateTimerHandle);
	FHitResult LocCursorHitResult;
	LocController->GetHitResultUnderCursorByChannel(TraceTypeQuery3, true, LocCursorHitResult);
	if(LocCursorHitResult.bBlockingHit)
	{
		FRotator LocToCursorRotator(0.f, UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), LocCursorHitResult.Location).Yaw, 0.f);
		GetMesh()->SetWorldRotation(LocToCursorRotator);
	}		
}

void ACorePlayerCharacter::RecoverStamina()
{
	if(PlayerStatusComponent->GetStatusStamina() >= PlayerStatusComponent->GetStatusPreviousStamina())
	{
		GetWorld()->GetTimerManager().ClearTimer(RecoverStaminaTimerHandle);
		RecoverStaminaTimerHandle.Invalidate();
		PlayerStatusComponent->SetStatusStamina(PlayerStatusComponent->GetStatusPreviousStamina());
	}
	else
	{
		PlayerStatusComponent->SpendPlayerStamina(false);
		if(PlayerAnimInstance->GetCurrentNonInputState() == ENonInputAnimState::Tired)
		{
			if(PlayerStatusComponent->GetStatusStamina() > PlayerStatusComponent->GetStatusPreviousStamina() * 0.3f)
				PlayerAnimInstance->NonInputAnimFinished();										
		}
	}
}

void ACorePlayerCharacter::RotateMeshAxisForward(bool InbMove)
{
	if(!bInputAble) return;
	if(InbMove)
	{
		if(!PlayerAnimInstance->bAction)
		{
			FRotator LocMeshRotator = GetMesh()->GetComponentRotation();
			FRotator LocActorRotator = GetActorRotation();
			GetMesh()->SetWorldRotation(FMath::RInterpTo(LocMeshRotator, LocActorRotator, 0.3f, 1.0f));
		}		
	}
	else
	{
		GetWorld()->GetTimerManager().SetTimer(MeshRotateCalibrateTimerHandle, this, &ACorePlayerCharacter::CalibrateMeshAxisForward, GetWorld()->DeltaTimeSeconds, true);
	}
}

bool ACorePlayerCharacter::CheckSkillCalibrateRange(const FSkill& InSkillData, AActor*& OutNearActor) const
{
	if(!CurrentUsingSkill) return false;
	if(InSkillData.fCorrectRange > 0)
	{
		SkillCalibrateAreaCollision->SetSphereRadius(InSkillData.fCorrectRange);
		SkillCalibrateAreaCollision->SetCollisionEnabled(ECollisionEnabled::Type::QueryOnly);
		TArray<AActor*> LocOverlapNpc;
		SkillCalibrateAreaCollision->GetOverlappingActors(LocOverlapNpc, ACoreNpc::StaticClass());
		SkillCalibrateAreaCollision->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
		TArray<AActor*> LocFixedNpcs;
		for(AActor* LocFixedNpc : LocOverlapNpc)
		{
			if(ASPNpcFixed* LocCastFixedNpc = Cast<ASPNpcFixed>(LocFixedNpc)) LocFixedNpcs.Add(LocCastFixedNpc);
		}
		if(LocFixedNpcs.Num()>0)
		for(AActor* LocRemoveFixedNpc : LocFixedNpcs)
		{
			LocOverlapNpc.Remove(LocRemoveFixedNpc);
		}
		if(LocOverlapNpc.Num()>0)
		{
			float LocClosestDistance = 10000;
			AActor* LocClosestActor = nullptr;
			for(AActor* LocNpc : LocOverlapNpc)
			{
				if(LocClosestDistance > FVector::DistXY(LocNpc->GetActorLocation(), GetActorLocation()))
				{
					LocClosestActor = LocNpc;
					LocClosestDistance = FVector::DistXY(GetActorLocation(), LocNpc->GetActorLocation());
				}
			}
			OutNearActor = LocClosestActor;
			return true;
		}
	}
	OutNearActor = nullptr;
	return false;
}

void ACorePlayerCharacter::CalibrateMeshNearActor(const AActor* InNearActor)
{
	FRotator LocToCursorRotator(0.f, UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), InNearActor->GetActorLocation()).Yaw, 0.f);
	MovingActionForward = LocToCursorRotator;
	SetActorRotation(LocToCursorRotator);
	RotateMeshAxisForward(false);
}

void ACorePlayerCharacter::CalibrateMeshMouseCursor()
{
	APlayerController* LocController = UGameplayStatics::GetPlayerController(GetWorld(),0);
	if(!LocController) return;
	GetWorld()->GetTimerManager().ClearTimer(MeshRotateCalibrateTimerHandle);
	FHitResult LocCursorHitResult;
	LocController->GetHitResultUnderCursorByChannel(TraceTypeQuery3, false, LocCursorHitResult);
	if(LocCursorHitResult.bBlockingHit)
	{
		FRotator LocToCursorRotator(0.f, UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), LocCursorHitResult.Location).Yaw, 0.f);
		MovingActionForward = LocToCursorRotator;
		SetActorRotation(LocToCursorRotator);
		RotateMeshAxisForward(false);
	}		
}

void ACorePlayerCharacter::CalibrateMeshAxisForward()
{
	FRotator LocCheckFinish;
	PlayerAnimInstance->bAction? LocCheckFinish = MovingActionForward : LocCheckFinish = GetActorRotation();
	if(UKismetMathLibrary::EqualEqual_RotatorRotator(GetMesh()->GetComponentRotation(), LocCheckFinish, 1.f))
	{
		GetWorld()->GetTimerManager().ClearTimer(MeshRotateCalibrateTimerHandle);
		MeshRotateCalibrateTimerHandle.Invalidate();
	}
	else
	{
		if(bMovingAction && PlayerAnimInstance->bAction)
		{
			FRotator LocMeshRotator = GetMesh()->GetComponentRotation();
			GetMesh()->SetWorldRotation(FMath::RInterpTo(LocMeshRotator, MovingActionForward, 0.5f, 1.0f));
		}
		else
		{
			FRotator LocMeshRotator = GetMesh()->GetComponentRotation();
			FRotator LocActorRotator = GetActorRotation();
			GetMesh()->SetWorldRotation(FMath::RInterpTo(LocMeshRotator, LocActorRotator, 0.5f, 1.0f));
		}		
	}
}

void ACorePlayerCharacter::CheckClosestObject()
{
	TArray<AActor*> OutActors;
	InteractionAreaCollision->GetOverlappingActors(OutActors, AActor::StaticClass());

	TArray<AActor*> FillteredActors = UGlobalUtilsLibrary::GetFilteredCurrentActors(OutActors, {GetClass(), ACoreNpc::StaticClass()/*, ALevelCastObject::StaticClass()*/},
	                                                                                {UObjectInteractionControl::StaticClass()});

	if(FillteredActors.IsEmpty()) return;

	FVector LocCharacterLocation = GetActorLocation();
	for(auto It = FillteredActors.CreateConstIterator(); It; ++It)
	{
		if(!InteractableObject)
		{
			InteractableObject = *It;
			continue;
		}
		float LocCurDistance = FVector::DistXY(LocCharacterLocation, (*It)->GetActorLocation());
		float LocClosestDistance = FVector::DistXY(LocCharacterLocation, InteractableObject->GetActorLocation());
		if(LocClosestDistance > LocCurDistance) InteractableObject = *It;
	}

	if(InteractableObject->IsA(ALevelCastObject::StaticClass()) == false)
	{
		IVisualizable::Execute_ShowVisual(InteractableObject);
	}

	FillteredActors.RemoveSwap(InteractableObject);

	for(AActor* FillteredActor : FillteredActors) IVisualizable::Execute_HideVisual(FillteredActor);
}

void ACorePlayerCharacter::InteractionAreaBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                                       const FHitResult& SweepResult)
{
	if(OtherActor->IsA(ACoreInteractionObject::StaticClass()) || OtherActor->Implements<UObjectInteractionControl>())
	{
		if(!GetWorld()->GetTimerManager().IsTimerActive(ClosestObjectTimerHandle))
			GetWorld()->GetTimerManager().SetTimer(ClosestObjectTimerHandle, this,
			                                       &ACorePlayerCharacter::CheckClosestObject,
			                                       GetWorld()->DeltaTimeSeconds, true);

		if(OtherActor->IsA(ALevelCastObject::StaticClass()))
		{
			ALevelCastObject* CastObject = Cast<ALevelCastObject>(OtherActor);
			if(!NMT_ENSURE(CastObject)) return;

			TArray<FPlayerGimmickSlotData> GimmickData = PlayerInventoryComponent->GetGimmickSlot();
			int SlotIDX = 0;
			for(auto FindData : GimmickData)
			{
				if(FindData.ItemRowName == FROM_INT(CastObject->GetCastItemID()))
				{
					OnRequestInteractIcon.Execute(FString(), false);
					IVisualizable::Execute_ShowVisual(OtherActor);
					return;
				}
				SlotIDX++;
			}

			IVisualizable::Execute_HideVisual(OtherActor);

			UDataDam* DD = IGlobalObjectProvider::Execute_GetDataDam(GetGameInstance());
			const UDataTable* FoundTable = DD->GetDataTableByEnum(EDataTableKey::Item);
			uint8* FoundData = FoundTable->FindRowUnchecked(FROM_INT(CastObject->GetCastItemID()));
			NMT_MSG_CHECKF(FoundData, "CastItemId[%s]: Failed to locate item data. Index [%s] may be out of range—confirm this entry in Item.csv.", FROM_INT(CastObject->GetCastItemID()),
			               FROM_INT(CastObject->GetCastItemID()));

			FItem* ItemData = reinterpret_cast<FItem*>(FoundData);
			OnRequestInteractIcon.Execute(ItemData->sItemIconPath, true);
		}
		else
		{
			if(OnRequestInteractIcon.IsBound()) OnRequestInteractIcon.Execute(FString(), false);
			if(RefreshInteractionVisibility.IsBound()) RefreshInteractionVisibility.Broadcast(true);
		}
	}
}

void ACorePlayerCharacter::InteractionAreaEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if(OtherActor->IsA(ACoreInteractionObject::StaticClass()) || OtherActor->Implements<UObjectInteractionControl>())
	{
		IVisualizable::Execute_HideVisual(OtherActor);
		TArray<AActor*> LocOverlappedActors;
		InteractionAreaCollision->GetOverlappingActors(LocOverlappedActors, AActor::StaticClass());

		TArray<AActor*> FillteredActors = UGlobalUtilsLibrary::GetFilteredCurrentActors(LocOverlappedActors, {GetClass(), ACoreNpc::StaticClass()},
		                                                                                {UObjectInteractionControl::StaticClass()});

		if(FillteredActors.IsEmpty())
		{
			InteractableObject = nullptr;
			GetWorld()->GetTimerManager().ClearTimer(ClosestObjectTimerHandle);
			ClosestObjectTimerHandle.Invalidate();
			if(RefreshInteractionVisibility.IsBound()) RefreshInteractionVisibility.Broadcast(false);
		}
	}
}

void ACorePlayerCharacter::NoiseAreaBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                                 const FHitResult& SweepResult)
{
	if(ACoreNpc* LocCurOverlapNpc = Cast<ACoreNpc>(OtherActor))
	{
		LocCurOverlapNpc->Alert(GetActorLocation());
		LocCurOverlapNpc->SetDetectTargetFromNoise(this);
	}
}

void ACorePlayerCharacter::NoiseAreaEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	NOT_IMPLEMENTED();	
}

void ACorePlayerCharacter::InitializeCharacterSet()
{
	NOT_IMPLEMENTED();
}

void ACorePlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACorePlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if(UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(PlayerActions.AxisMoveAction, ETriggerEvent::Started, this, &ACorePlayerCharacter::AxisMoveStarted);
		EnhancedInputComponent->BindAction(PlayerActions.AxisMoveAction, ETriggerEvent::Triggered, this, &ACorePlayerCharacter::AxisMoveTriggered);
		EnhancedInputComponent->BindAction(PlayerActions.AxisMoveAction, ETriggerEvent::Completed, this, &ACorePlayerCharacter::AxisMoveCompleted);
		EnhancedInputComponent->BindAction(PlayerActions.RunAction, ETriggerEvent::Triggered, this, &ACorePlayerCharacter::RunTriggered);
		EnhancedInputComponent->BindAction(PlayerActions.RunAction, ETriggerEvent::Completed, this, &ACorePlayerCharacter::RunCompleted);
		EnhancedInputComponent->BindAction(PlayerActions.MeleeAttackAction, ETriggerEvent::Started, this, &ACorePlayerCharacter::ActivateMeleeAttack);
		EnhancedInputComponent->BindAction(PlayerActions.SkillAction, ETriggerEvent::Started, this, &ACorePlayerCharacter::ActivateSkill);
		EnhancedInputComponent->BindAction(PlayerActions.InteractionAction, ETriggerEvent::Started, this, &ACorePlayerCharacter::InteractionStarted);
		EnhancedInputComponent->BindAction(PlayerActions.UseSlot1Action, ETriggerEvent::Started, this, &ACorePlayerCharacter::UseItemSlot1);
		EnhancedInputComponent->BindAction(PlayerActions.UseSlot2Action, ETriggerEvent::Started, this, &ACorePlayerCharacter::UseItemSlot2);
		EnhancedInputComponent->BindAction(PlayerActions.UseSlot3Action, ETriggerEvent::Started, this, &ACorePlayerCharacter::UseItemSlot3);
		EnhancedInputComponent->BindAction(PlayerActions.PauseAction, ETriggerEvent::Started, this, &ACorePlayerCharacter::ToggleGameMenu);
		
		EnhancedInputComponent->BindAction(PlayerDebugActions.DebugCharacterSelect, ETriggerEvent::Started, this, &ACorePlayerCharacter::DebugCharacterSelect);
		EnhancedInputComponent->BindAction(PlayerDebugActions.DebugToggleCheat, ETriggerEvent::Started, this, &ACorePlayerCharacter::DebugToggleCheat);
		EnhancedInputComponent->BindAction(PlayerDebugActions.DebugToggleFrustum, ETriggerEvent::Started, this, &ACorePlayerCharacter::DebugToggleFrustum);
		EnhancedInputComponent->BindAction(PlayerDebugActions.DebugToggleInvincible, ETriggerEvent::Started, this, &ACorePlayerCharacter::DebugToggleInvincible);
		EnhancedInputComponent->BindAction(PlayerDebugActions.DebugToggleSlomo, ETriggerEvent::Started, this, &ACorePlayerCharacter::DebugToggleSlomo);
		EnhancedInputComponent->BindAction(PlayerDebugActions.DebugToggleHitCollision, ETriggerEvent::Started, this, &ACorePlayerCharacter::DebugToggleHitCollision);
		EnhancedInputComponent->BindAction(PlayerDebugActions.DebugTogglePlayerDamage, ETriggerEvent::Started, this, &ACorePlayerCharacter::DebugTogglePlayerDamage);
		EnhancedInputComponent->BindAction(PlayerDebugActions.DebugTogglePlayerSpeed, ETriggerEvent::Started, this, &ACorePlayerCharacter::DebugTogglePlayerSpeed);
		EnhancedInputComponent->BindAction(PlayerDebugActions.DebugToggleTimeStop, ETriggerEvent::Started, this, &ACorePlayerCharacter::DebugToggleTimeStop);
		EnhancedInputComponent->BindAction(PlayerDebugActions.DebugToggleCam, ETriggerEvent::Started, this, &ACorePlayerCharacter::DebugToggleSpectatorPawn);
		
		CurrentEnhancedInputComponent = EnhancedInputComponent;
		PlayerDeadDone.AddDynamic(this, &ACorePlayerCharacter::EventOnDead);
	}
	else
	{
		NMT_MSG_CHECKF(0, "Fail to set input")
	}
}

void ACorePlayerCharacter::ApplyDamageEvent_Implementation(AActor* InOccurActor, const FVector& InOriginLocation, const float InDamage, const TArray<FEffect>& InSkillEffects)
{
	UCoreCheatManager* CCM = UGlobalUtilsLibrary::GetSharedCoreCheatManager(this);
	float FinalDamage = CCM->GetDebugStatus().bDebugPlayerDamage? InDamage + 1000.f : InDamage;
	
	if(PlayerStatusComponent->ApplyDamage(FinalDamage))
	{
		if(bDead || bInvincible) return;
		bDead = true;
		PlayerAnimInstance->SetNonInputAnimState(ENonInputAnimState::Dead);
		UNotificationCenter* LocNotiCenter = IGlobalObjectProvider::Execute_GetNotificationCenter(UGameplayStatics::GetGameInstance(this));
		LocNotiCenter->Post(ENotificationTypes::Player_Dead);
		if(PlayerDeadDone.IsBound()) PlayerDeadDone.Broadcast();
		RespawnCharacter();
	}
	else
	{
		if(CCM->GetDebugStatus().bDebugPlayerInvincibleStatusEffect) return;
		if(InSkillEffects.Num() >= 1)
		{
			for(FEffect LocCurEffect : InSkillEffects)
			{
				EEffectSubTypes LocCurEffectType = UGlobalUtilsLibrary::GetStringToEnum<EEffectSubTypes>(LocCurEffect.sEffect);
				if(CheckEffectTime(LocCurEffect))
				{
					switch(LocCurEffectType)
					{
						case EEffectSubTypes::Hitback:
						{
							if(CurrentUsingSkill != nullptr)
							{
								StopActionMontage();
								FSkill LocSkillData;
								CurrentUsingSkill->GetSkillData(LocSkillData);
								CurrentUsingSkill.GetClass() == PlayerAtkSkillClass
									? SetMeleeAtkSkillState(EPlayerSkillState::Cancel, LocSkillData)
									: SetSkillState(EPlayerSkillState::Cancel, LocSkillData);
							}
							PlayerAnimInstance->SetNonInputAnimState(ENonInputAnimState::HitBack);
							break;
						}
						case EEffectSubTypes::Jumpback:
						{
							if(CurrentUsingSkill != nullptr)
							{
								StopActionMontage();
								FSkill LocSkillData;
								CurrentUsingSkill->GetSkillData(LocSkillData);
								CurrentUsingSkill.GetClass() == PlayerAtkSkillClass
									? SetMeleeAtkSkillState(EPlayerSkillState::Cancel, LocSkillData)
									: SetSkillState(EPlayerSkillState::Cancel, LocSkillData);
							}
							if(!bInvincible)
							{
								bInvincible = true;
								PlayerJumpBack(nullptr);
							}
							break;
						}
						case EEffectSubTypes::Unrecog:
						{
							NOT_IMPLEMENTED_MSG("EEffectSubTypes::Unrecog");
							break;
						}
						case EEffectSubTypes::Fast:
						{
							StatusFast(LocCurEffect.fEffectValue);
							break;
						}
						case EEffectSubTypes::Slow:
						{
							StatusSlow(LocCurEffect.fEffectValue);
							break;
						}
						case EEffectSubTypes::CogImmune:
						{
							SetUnDetectState(true);
							break;
						}
						case EEffectSubTypes::Heal:
						{
							if(PlayerStatusComponent->GetStatusPreviousCharHp() > PlayerStatusComponent->GetStatusCharHp() + LocCurEffect.fEffectValue)
							{
								PlayerStatusComponent->SetStatusCharHp(PlayerStatusComponent->GetStatusCharHp() + LocCurEffect.fEffectValue);
							}
							else
							{
								PlayerStatusComponent->SetStatusCharHp(PlayerStatusComponent->GetStatusPreviousCharHp());
							}
							PlayerStatusComponent->UpdatePlayerHpPercent.Broadcast(PlayerStatusComponent->GetStatusCharHp() / PlayerStatusComponent->GetStatusPreviousCharHp());
						}
						default: NMT_LOG("No Effect Type");
					}
					AddEffectTimeMap(LocCurEffect);
				}
			}
		}
	}
}

void ACorePlayerCharacter::ApplyRecoveryEvent_Implementation(const TArray<FEffect>& InReleaseEffects)
{
	
	for(FEffect LocCurReleaseEffect : InReleaseEffects)
	{
		EEffectSubTypes LocCurRecoveryTypes = UGlobalUtilsLibrary::GetStringToEnum<EEffectSubTypes>(LocCurReleaseEffect.sEffect);
		EffectTimeMap.Remove(LocCurRecoveryTypes);
		if(EffectTimeMap.IsEmpty())
		{
			GetWorld()->GetTimerManager().ClearTimer(EffectActiveTimerHandle);
			if(RefreshEffectTime.IsBound()) RefreshEffectTime.Broadcast(LocCurRecoveryTypes, 0.f, true);
		}
		switch(LocCurRecoveryTypes)
		{
			case EEffectSubTypes::Hitback:
			{
				NOT_IMPLEMENTED_MSG("Release EEffectSubTypes::Hitback");
				break;
			}
			case EEffectSubTypes::Jumpback:
			{
				NOT_IMPLEMENTED_MSG("Release EEffectSubTypes::Jumpback");
				break;
			}
			case EEffectSubTypes::Unrecog:
			{
				NOT_IMPLEMENTED_MSG("Release EEffectSubTypes::Unrecog");
				break;
			}
			case EEffectSubTypes::Fast:
			{
				StatusFast(LocCurReleaseEffect.fEffectValue, true);
				break;	
			}
			case EEffectSubTypes::Slow:
			{
				StatusSlow(LocCurReleaseEffect.fEffectValue, true);
				break;
			}
			case EEffectSubTypes::CogImmune:
			{
				SetUnDetectState(false);
				break;
			}
			default: NMT_LOG("No Release Effect Type");
		}
	}
}

void ACorePlayerCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorld()->GetTimerManager().ClearTimer(SkillCoolDownTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(EffectActiveTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(ClosestObjectTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(DecreaseStaminaTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(RecoverStaminaTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(MeshRotateCalibrateTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(FadeTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(JumpBackTimerHandle);
	Super::EndPlay(EndPlayReason);
}

void ACorePlayerCharacter::SetSkillState(EPlayerSkillState InSkillState, FSkill InSkillData)
{
	switch(InSkillState)
	{
		case EPlayerSkillState::Start:
		{
			if(bMovingAction)
			{
				PlayerStatusComponent->SetStatusCharMove(PlayerStatusComponent->GetStatusCharMove() * SkillDecreaseSpeedRatio);
				PlayerAnimInstance->SetActionState(true);
			}
			else
			{
				SetInputable(false);	
			}
			CurrentUsingSkill->StartSkill();
			break;
		}
		case EPlayerSkillState::Aim: break;
		case EPlayerSkillState::Cancel:
		{
			SetSkillCoolTime(CurrentUsingSkill.GetClass(),InSkillData);
			CurrentUsingSkill->CancelSkill();
			SetCurrentSkill(false);
			PlayerAnimInstance->SetActionState(false);
			PlayerStatusComponent->SetStatusCharMove(PlayerStatusComponent->GetStatusPreviousCharMove());
			break;
		}
		case EPlayerSkillState::Release:
		{
			CurrentUsingSkill->CancelSkill();
			SetCurrentSkill(false);
			if(bMovingAction)
			{
				PlayerStatusComponent->SetStatusCharMove(PlayerStatusComponent->GetStatusPreviousCharMove());
				PlayerAnimInstance->SetActionState(false);				
			}
			else
			{
				PlayerAnimInstance->SetActionState(false);
				SetInputable(true);	
			}			
			break;
		}
		case EPlayerSkillState::Activate: break;
		case EPlayerSkillState::Finish:	break;
		default: NMT_LOG("PlayerSkill State None"); break;
	}
}

void ACorePlayerCharacter::SetMeleeAtkSkillState(EPlayerSkillState InSkillState, FSkill InSkillData)
{
	switch(InSkillState)
	{
		case EPlayerSkillState::Start:
		{
			if(bCalibrateSkill)
			{
				AActor* LocNearActor = nullptr;
				if(CheckSkillCalibrateRange(InSkillData,LocNearActor))
				{
					CalibrateMeshNearActor(LocNearActor);
				}
				else
				{
					#if ONLY_KEYBOARD
					CalibrateMeshMouseCursor();
					#endif
				}
			}
			else
			{
				#if ONLY_KEYBOARD
				CalibrateMeshMouseCursor();
				#endif	
			}
				
			if(bMovingAction)
			{
				PlayerStatusComponent->SetStatusCharMove(PlayerStatusComponent->GetStatusCharMove() * MeleeAtkDecreaseSpeedRatio);
			}
			else
			{
				SetInputable(false);	
			}
			CurrentUsingSkill->StartSkill();
			PlayActionMontage(*InSkillData.sSkillName,FOnMontageEnded::CreateLambda([InSkillData, this](UAnimMontage* Montage, bool Interrupted)
			{
				if(!Interrupted) SetMeleeAtkSkillState(EPlayerSkillState::Finish,InSkillData);
			}));
			break;
		}
		case EPlayerSkillState::Aim: break;
		case EPlayerSkillState::Cancel:
		{
			SetSkillCoolTime(CurrentUsingSkill.GetClass(), InSkillData);
			CurrentUsingSkill->CancelSkill();
			SetCurrentSkill(false);
			PlayerAnimInstance->SetActionState(false);
			if(bMovingAction)
			{
				SetActorRotation(GetMesh()->GetComponentRotation());
				PlayerStatusComponent->SetStatusCharMove(PlayerStatusComponent->GetStatusPreviousCharMove());
			}
			else
			{
				SetInputable(true);	
			}
			break;
		}
		case EPlayerSkillState::Release: break;
		case EPlayerSkillState::Finish:
		{
			SetSkillCoolTime(CurrentUsingSkill.GetClass(), InSkillData);
			CurrentUsingSkill->FinishSkill();
			SetCurrentSkill(false);
			PlayerAnimInstance->SetActionState(false);
			if(bMovingAction)
			{
				SetActorRotation(GetMesh()->GetComponentRotation());
				PlayerStatusComponent->SetStatusCharMove(PlayerStatusComponent->GetStatusPreviousCharMove());
			}
			else
			{
				SetInputable(true);	
			}
			break;
		}
	default: NMT_LOG("PlayerAtk State None"); break;
	}
}

void ACorePlayerCharacter::SetItemSkillState(EPlayerSkillState InSkillState, FSkill InSkillData)
{
	// TODO: Edit Immediately Skill, AimSkill Set From Type
	switch(InSkillState)
	{
		case EPlayerSkillState::Start:
		{
			#if ONLY_KEYBOARD
			//CalibrateMeshMouseCursor();
			#endif
			//FIXME : Const Item Aim Decrease Value Speed Edit 
			PlayerStatusComponent->SetStatusCharMove(PlayerStatusComponent->GetStatusCharMove() * 0.5f);
			//SetInputable(false);
			//PlayCommonMontage(*InSkillData.sSkillName, nullptr);
			//SetItemSkillState(EPlayerSkillState::Aim);
			CurrentUsingSkill->StartSkill();
			ESkillTypes LocItemSkillType = UGlobalUtilsLibrary::GetStringToEnum<ESkillTypes>(InSkillData.sSkillType);
			if(LocItemSkillType == ESkillTypes::Indirect || LocItemSkillType == ESkillTypes::Direct)
			SetItemSkillState(EPlayerSkillState::Aim, InSkillData);	
			break;
		}
		case EPlayerSkillState::Aim:
		{
			ESkillTypes LocItemSkillType = UGlobalUtilsLibrary::GetStringToEnum<ESkillTypes>(InSkillData.sSkillType);
			if(LocItemSkillType == ESkillTypes::Indirect)
			{
				StartMouseCursorParabola(true);	
			}
			else if(LocItemSkillType == ESkillTypes::Direct)
			{
				StartMouseCursorLinear(true);
			}
			CurrentUsingSkill->AimSkill();
			if(bMovingAction)
			{
				PlayerAnimInstance->SetActionState(false);
			}
			else
			{
				SetInputable(false);
				GetWorld()->GetTimerManager().ClearTimer(MeshRotateCalibrateTimerHandle);
				GetWorld()->GetTimerManager().SetTimer(MouseCursorTimerHandle, this, &ACorePlayerCharacter::RotateMeshToMouseCursor, GetWorld()->DeltaTimeSeconds, true);
			}
			break;
		}
		case EPlayerSkillState::Cancel:
		{
			StartMouseCursorParabola(false);	
			StartMouseCursorLinear(false);
			GetWorld()->GetTimerManager().ClearTimer(MouseCursorTimerHandle);
			CurrentUsingSkill->CancelSkill();
			SetItemSkillState(EPlayerSkillState::Finish);
			PlayerStatusComponent->SetStatusCharMove(PlayerStatusComponent->GetStatusPreviousCharMove());
			break;
		}
		case EPlayerSkillState::Release:
		{
			StartMouseCursorParabola(false);	
			StartMouseCursorLinear(false);
			GetWorld()->GetTimerManager().ClearTimer(MouseCursorTimerHandle);
			CurrentUsingSkill->ReleaseSkill();
			SetItemSkillState(EPlayerSkillState::Finish);
			PlayerStatusComponent->SetStatusCharMove(PlayerStatusComponent->GetStatusPreviousCharMove());
			break;
		}
		case EPlayerSkillState::Activate:
		{
			StartMouseCursorParabola(false);	
			StartMouseCursorLinear(false);
			GetWorld()->GetTimerManager().ClearTimer(MouseCursorTimerHandle);
			CurrentUsingSkill->ActivateSkill();
			ESkillTypes LocItemSkillType = UGlobalUtilsLibrary::GetStringToEnum<ESkillTypes>(InSkillData.sSkillType);
			int LocSlotNumber;
			GetSlotCursor(LocSlotNumber);
			GetPlayerInventory()->EmptySpendSlotItem(LocSlotNumber);
			SetSlotCursor(-1);
			if(LocItemSkillType != ESkillTypes::Buff)
			{
				FName LocActivateSection = TEXT("CommonItemThrow");
				PlayCommonMontage(LocActivateSection, FOnMontageEnded::CreateLambda([this](UAnimMontage* Montage, bool Interrupted)
				{
					if(!Interrupted) SetItemSkillState(EPlayerSkillState::Finish);				
				}));	
				SetActorRotationToMouseCursor();
				CalibrateMeshMouseCursor();
				SetInputable(false);	
			}
			else
			{
				SetItemSkillState(EPlayerSkillState::Finish);
			}
			break;
		}
		case EPlayerSkillState::Finish:
		{
			SetCurrentSkill(false);
			PlayerAnimInstance->SetActionState(false);
			PlayerStatusComponent->SetStatusCharMove(PlayerStatusComponent->GetStatusPreviousCharMove());
			SetInputable(true);
			break;
		}
		default: NMT_LOG("PlayerItem State None"); break;
	}
}

void ACorePlayerCharacter::SetupPlayerInputActions()
{
	//TODO : Specific Path -> Relative Data Path
	DefaultMappingContext = UGlobalUtilsLibrary::FindObject<UInputMappingContext>("/Script/EnhancedInput.InputMappingContext'/Game/System/Core/Input/InputMappingContext/IMC_LocalPlayerV1.IMC_LocalPlayerV1'");

	auto LocFindInputAction = UGlobalUtilsLibrary::FindObject<UInputAction>;
	PlayerActions.AxisMoveAction = LocFindInputAction("/Script/EnhancedInput.InputAction'/Game/System/Core/Input/InputAction/IA_AxisMove.IA_AxisMove'");
	PlayerActions.RunAction = LocFindInputAction("/Script/EnhancedInput.InputAction'/Game/System/Core/Input/InputAction/IA_Run.IA_Run'");
	PlayerActions.MeleeAttackAction = LocFindInputAction("/Script/EnhancedInput.InputAction'/Game/System/Core/Input/InputAction/IA_MeleeAttack.IA_MeleeAttack'");
	PlayerActions.SkillAction = LocFindInputAction("/Script/EnhancedInput.InputAction'/Game/System/Core/Input/InputAction/IA_ActiveSkill.IA_ActiveSkill'");
	PlayerActions.InteractionAction = LocFindInputAction("/Script/EnhancedInput.InputAction'/Game/System/Core/Input/InputAction/IA_Interaction.IA_Interaction'");
	PlayerActions.UseSlot1Action = LocFindInputAction("/Script/EnhancedInput.InputAction'/Game/System/Core/Input/InputAction/IA_Slot1.IA_Slot1'");
	PlayerActions.UseSlot2Action = LocFindInputAction("/Script/EnhancedInput.InputAction'/Game/System/Core/Input/InputAction/IA_Slot2.IA_Slot2'");
	PlayerActions.UseSlot3Action = LocFindInputAction("/Script/EnhancedInput.InputAction'/Game/System/Core/Input/InputAction/IA_Slot3.IA_Slot3'");
	PlayerActions.MinimapAction = LocFindInputAction("/Script/EnhancedInput.InputAction'/Game/System/Core/Input/InputAction/IA_ToggleMap.IA_ToggleMap'");
	PlayerActions.PauseAction = LocFindInputAction("/Script/EnhancedInput.InputAction'/Game/System/Core/Input/InputAction/IA_Pause.IA_Pause'");

	PlayerDebugActions.DebugCharacterSelect = LocFindInputAction("/Script/EnhancedInput.InputAction'/Game/System/Core/Input/InputAction/CheatAction/IA_DebugCharacterSelect.IA_DebugCharacterSelect'");
	PlayerDebugActions.DebugToggleCheat = LocFindInputAction("/Script/EnhancedInput.InputAction'/Game/System/Core/Input/InputAction/CheatAction/IA_DebugToggleCheat.IA_DebugToggleCheat'");
	PlayerDebugActions.DebugToggleFrustum = LocFindInputAction("/Script/EnhancedInput.InputAction'/Game/System/Core/Input/InputAction/CheatAction/IA_DebugToggleFrustum.IA_DebugToggleFrustum'");
	PlayerDebugActions.DebugToggleInvincible = LocFindInputAction("/Script/EnhancedInput.InputAction'/Game/System/Core/Input/InputAction/CheatAction/IA_DebugToggleInvincible.IA_DebugToggleInvincible'");
	PlayerDebugActions.DebugToggleSlomo = LocFindInputAction("/Script/EnhancedInput.InputAction'/Game/System/Core/Input/InputAction/CheatAction/IA_DebugToggleSlomo.IA_DebugToggleSlomo'");
	PlayerDebugActions.DebugToggleHitCollision = LocFindInputAction("/Script/EnhancedInput.InputAction'/Game/System/Core/Input/InputAction/CheatAction/IA_DebugToggleHitCollision.IA_DebugToggleHitCollision'");
	PlayerDebugActions.DebugTogglePlayerDamage = LocFindInputAction("/Script/EnhancedInput.InputAction'/Game/System/Core/Input/InputAction/CheatAction/IA_DebugTogglePlayerDamage.IA_DebugTogglePlayerDamage'");
	PlayerDebugActions.DebugTogglePlayerSpeed = LocFindInputAction("/Script/EnhancedInput.InputAction'/Game/System/Core/Input/InputAction/CheatAction/IA_DebugTogglePlayerSpeed.IA_DebugTogglePlayerSpeed'");
	PlayerDebugActions.DebugToggleTimeStop = LocFindInputAction("/Script/EnhancedInput.InputAction'/Game/System/Core/Input/InputAction/CheatAction/IA_DebugToggleTimeStop.IA_DebugToggleTimeStop'");
	PlayerDebugActions.DebugToggleCam = LocFindInputAction("/Script/EnhancedInput.InputAction'/Game/System/Core/Input/InputAction/CheatAction/IA_DebugToggleCam.IA_DebugToggleCam'");

	PlayerChordedActions.CtrlAction = LocFindInputAction("/Script/EnhancedInput.InputAction'/Game/System/Core/Input/InputAction/CheatAction/IA_DebugCharacterSelect.IA_DebugCharacterSelect'");
}

void ACorePlayerCharacter::EventOnDead()
{
	CurrentEnhancedInputComponent->ClearActionBindings();
}

void ACorePlayerCharacter::UpdateRequestInteractionIcon(const FOnRequestInteractIcon& Callback)
{
	if(!Callback.IsBound()) return;
	OnRequestInteractIcon = Callback;
}

#if WITH_EDITOR
void ACorePlayerCharacter::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	ApplyCharacterConfiguration(CharacterRowName);
}

void ACorePlayerCharacter::ApplyCharacterData(const FTableRowBase* RowData)
{
	if(!GEditor) return;
	if(!RowData)
	{
		PlayerStatusComponent->InitializeStatusData(FPlayerCharacter());
		return;
	}
	const FPlayerCharacter* GetData = static_cast<const FPlayerCharacter*>(RowData);
	NMT_CHECKF(GetData);

	if(*GetData == PlayerStatusComponent->CurrentPlayerStatus) return;
	if(GEditor->GetEditorWorldContext().World())
	{
		PlayerStatusComponent->InitializeStatusData(*GetData);
		SetSkillClass();
	}
}
#endif
