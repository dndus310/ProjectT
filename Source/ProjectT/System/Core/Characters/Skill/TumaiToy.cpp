// Fill out your copyright notice in the Description page of Project Settings.


#include "TumaiToy.h"

#include "NiagaraComponent.h"
#include "Animations/AnimInstance/SkillObjectAnimInstance.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "ProjectT/System/Core/Characters/Player/CorePlayerCharacter.h"
#include "ProjectT/System/Core/Common/GlobalUtilsLibrary.h"
#include "ProjectT/System/Core/Managers/SoundManager.h"

ATumaiToy::ATumaiToy() :
	TumaiToyRootScene(nullptr),
	MeshRootSceneComponent(nullptr),
	TumaiToyMeshComponent(nullptr),
	TumaiToyHitCollisionComponent(nullptr),
	TumaiToyIdleAnimSequence(nullptr),
	TumaiToyMontage(nullptr),
	DissolveMaterialInstance(nullptr),
	TumaiToySFXComponent(nullptr),
	ProjectileMovementComponent(nullptr),
	CurrentActiveTime(0.f),
	DissolveValue(1.f),
	bHit(false)
{
	SkillPoolsize = 5;
	SkillRowName = "313302";
	SkillId = 313302;
	InitializeSkillData();
	PrimaryActorTick.bCanEverTick = true;

	TumaiToyRootScene = CreateDefaultSubobject<USceneComponent>(TEXT("TumaiToyRootScene"));
	MeshRootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("MeshRootScene"));
	TumaiToyMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("TumaiToyMeshComponent"));
	TumaiToyHitCollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("TumaiToyHitCollisionComponent"));
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));

	TumaiToyRootScene->SetupAttachment(RootComponent);
	MeshRootSceneComponent->SetupAttachment(RootComponent);
	
	TumaiToyMeshComponent->SetupAttachment(MeshRootSceneComponent);
	TumaiToyMeshComponent->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	TumaiToyMeshComponent->SetHiddenInGame(true);

	TumaiToyHitCollisionComponent->SetupAttachment(MeshRootSceneComponent);
	TumaiToyHitCollisionComponent->SetCollisionProfileName(TEXT("PlayerHit"));
	TumaiToyHitCollisionComponent->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	TumaiToyHitCollisionComponent->SetSphereRadius(1.f);
	TumaiToyHitCollisionComponent->OnComponentBeginOverlap.AddDynamic(this,&ATumaiToy::DefaultCollisionBeginOverlap);

	TumaiToyMeshComponent->SetHiddenInGame(true);
	
	ProjectileMovementComponent->SetUpdatedComponent(RootComponent);
	ProjectileMovementComponent->bAutoActivate = false;
	ProjectileMovementComponent->Deactivate();

	BodyHitComponent = TumaiToyHitCollisionComponent;
	RangeHitComponent = NoiseSphereComponent;
}

void ATumaiToy::BeginPlay()
{
	Super::BeginPlay();
	if(USkillObjectAnimInstance* LocAnimInstance = Cast<USkillObjectAnimInstance>(TumaiToyMeshComponent->GetAnimInstance()))
		LocAnimInstance->SetDefaultProperty(TumaiToyIdleAnimSequence, TumaiToyMontage);
	if(DissolveMaterialInstance) TumaiToyMeshComponent->SetMaterial(0, DissolveMaterialInstance);
}

void ATumaiToy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ATumaiToy::Enable_Implementation()
{
	Super::Enable_Implementation();
}

void ATumaiToy::Disable_Implementation()
{
	Super::Disable_Implementation();
	ResetDefaultState();
}

void ATumaiToy::Pushed_Implementation()
{
	Super::Pushed_Implementation();
}

void ATumaiToy::Obtained_Implementation()
{
	Super::Obtained_Implementation();
}

void ATumaiToy::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if(UWorld* LocWorld = UGlobalUtilsLibrary::GetValidWorld(this))
	{
		LocWorld->GetTimerManager().ClearTimer(RotatorTimerHandle);
		LocWorld->GetTimerManager().ClearTimer(FXDeactivateTimerHandle);
		LocWorld->GetTimerManager().ClearTimer(DissolveTimerHandle);
	}
	Super::EndPlay(EndPlayReason);
}

void ATumaiToy::StartSkill()
{
	if(bAimState) ReleaseSkill();
}

void ATumaiToy::AimSkill()
{
	bCanceled = false;
	bAimState = true;
	AttachToActor(SkillOwner, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	if(USkillObjectAnimInstance* LocAnimInstance = Cast<USkillObjectAnimInstance>(TumaiToyMeshComponent->GetAnimInstance()))
	{
		LocAnimInstance->Montage_Play(LocAnimInstance->GetSkillObjectMontage());
	}
	ACombatCharacter* LocPlayerCharacter = Cast<ACorePlayerCharacter>(SkillOwner);
	TumaiToyMeshComponent->AttachToComponent(LocPlayerCharacter->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale,TEXT("hand_r"));
	TumaiToyMeshComponent->SetHiddenInGame(false);
	SetActorTickEnabled(true);
}

void ATumaiToy::ActivateSkill()
{
	if(bActivated) return;
	bActivated = true;
	bAimState = false;
}

void ATumaiToy::ReleaseSkill()
{
	bCanceled = true;
	bReleased = true;
	FinishSkill();
}

void ATumaiToy::CancelSkill()
{
	bCanceled = true;
	FinishSkill();
}

void ATumaiToy::FinishSkill()
{
	Super::FinishSkill();
}

void ATumaiToy::SkillHitEnable(bool InbEnable)
{
	if(InbEnable)
	{
		ProjectileTumaiToy();
		TumaiToyMeshComponent->AttachToComponent(MeshRootSceneComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		TumaiToyHitCollisionComponent->SetCollisionEnabled(ECollisionEnabled::Type::QueryOnly);
		ACorePlayerCharacter* LocPlayerCharacter = Cast<ACorePlayerCharacter>(SkillOwner);
		LocPlayerCharacter->SetCurrentSkill(false);
	}
	else
	{
		ProjectileMovementComponent->Deactivate();
		TumaiToyHitCollisionComponent->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
		RootComponent->SetRelativeRotation(FRotator(0.f, RootComponent->GetRelativeRotation().Yaw, 0.f));
		if(USkillObjectAnimInstance* LocAnimInstance = Cast<USkillObjectAnimInstance>(TumaiToyMeshComponent->GetAnimInstance()))
		{
			LocAnimInstance->Montage_Play(LocAnimInstance->GetSkillObjectMontage());
			LocAnimInstance->Montage_JumpToSection(TEXT("GetUp"),LocAnimInstance->GetSkillObjectMontage());
		}
	}
}

void ATumaiToy::SkillObjectHitEnable(bool InbEnable)
{
	if(InbEnable)
	{
		MakeSkillNoise(true,TumaiToyMeshComponent);
		NoiseFXComponent->SetWorldLocation(FVector(NoiseFXComponent->GetComponentLocation().X, NoiseFXComponent->GetComponentLocation().Y, 0.1f));
		TumaiToySFXComponent = UGlobalUtilsLibrary::GetSharedSoundManager(this)->PlaySound3DAttached(TEXT("831020"), RootComponent, EAttachLocation::Type::SnapToTarget,TEXT("None"));
		if(UWorld* LocWorld = UGlobalUtilsLibrary::GetValidWorld(this))
		{
			LocWorld->GetTimerManager().SetTimer(FXDeactivateTimerHandle, this, &ATumaiToy::CheckFXDeactivated, LocWorld->DeltaRealTimeSeconds, true);
		}
	}
}

void ATumaiToy::ResetDefaultState()
{
	Super::ResetDefaultState();
	SetActorTickEnabled(false);
	if(SkillOwner != nullptr) SkillOwner = nullptr;
	CurrentActiveTime = 0.f;
	bHit = false;
	bCanceled = false;
	bAimState = false;
	bActivated = false;
	bReleased = false;
	TumaiToyMeshComponent->SetHiddenInGame(true);
	DissolveValue = 1.f;
	TumaiToyMeshComponent->SetScalarParameterValueOnMaterials(TEXT("Dissolve"), DissolveValue);
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
}

void ATumaiToy::ProjectileTumaiToy()
{
	if (SkillOwner)
	{
		DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		TumaiToyMeshComponent->AttachToComponent(TumaiToyRootScene, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		SetParabolaLaunchVelocity();
		ProjectileMovementComponent->Velocity = LaunchVelocity;
		InitLocation = GetActorLocation();
		ProjectileMovementComponent->Activate();
		if(UWorld* LocWorld = UGlobalUtilsLibrary::GetValidWorld(this))
		{
			LocWorld->GetTimerManager().SetTimer(RotatorTimerHandle, this, &ATumaiToy::RotateTumaiToyMesh, LocWorld->DeltaTimeSeconds, true);
		}
	}
}

void ATumaiToy::ExplosionTumaiToy()
{
	bHit = true;
	if(UWorld* LocWorld = UGlobalUtilsLibrary::GetValidWorld(this)) LocWorld->GetTimerManager().ClearTimer(RotatorTimerHandle);
	SkillHitEnable(false);
}

void ATumaiToy::RotateTumaiToyMesh() const
{
	FRotator LocAddRotator(10.f,0.f,0.f);
	RootComponent->AddLocalRotation(LocAddRotator);
}

void ATumaiToy::CheckFXDeactivated()
{
	if(NoiseFXComponent->IsActive()) return;
	if(UWorld* LocWorld = UGlobalUtilsLibrary::GetValidWorld(this))
	{
		LocWorld->GetTimerManager().ClearTimer(FXDeactivateTimerHandle);
		LocWorld->GetTimerManager().SetTimer(DissolveTimerHandle, this, &ATumaiToy::CheckDissolveDeactivated, LocWorld->DeltaTimeSeconds, true);
	}
}

void ATumaiToy::CheckDissolveDeactivated()
{
	if(UWorld* LocWorld = UGlobalUtilsLibrary::GetValidWorld(this))
	{
		DissolveValue - LocWorld->DeltaTimeSeconds < 0.f ? DissolveValue = 0.f : DissolveValue = DissolveValue - LocWorld->DeltaTimeSeconds;
		TumaiToyMeshComponent->SetScalarParameterValueOnMaterials(TEXT("Dissolve"), DissolveValue);
		if(DissolveValue <= 0.f)
		{
			LocWorld->GetTimerManager().ClearTimer(DissolveTimerHandle);
			FinishSkill();
		}		
	}	
}

void ATumaiToy::DefaultCollisionBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                             const FHitResult& SweepResult)
{
	if (!bHit && OtherComp->GetCollisionProfileName() == TEXT("Ground")) ExplosionTumaiToy();
}

