// Fill out your copyright notice in the Description page of Project Settings.


#include "SmokeShell.h"
#include "NiagaraComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "ProjectT/System/Core/Characters/NPC/CoreNpc.h"
#include "ProjectT/System/Core/Characters/Player/CorePlayerCharacter.h"
#include "ProjectT/System/Core/Common/GlobalUtilsLibrary.h"
#include "ProjectT/System/Core/GameModes/WorldGameMode.h"
#include "ProjectT/System/Core/Managers/SoundManager.h"

ASmokeShell::ASmokeShell() :
	SmokeShellRootScene(nullptr),
	ThrowFXComponent(nullptr),
	ExplosionFXComponent(nullptr),
	SmokeShellMeshComponent(nullptr),
	SmokeShellHitCollisionComponent(nullptr),
	UnDetectCollisionComponent(nullptr),
	SmokeShellSFXComponent(nullptr),
	ProjectileMovementComponent(nullptr),
	CurrentActiveTime(0.f),
	bHit(false),
	CurrentOverlapPlayer(nullptr)
{
	SkillPoolsize = 5;
	SkillRowName = "313301";
	SkillId = 313301;
	InitializeSkillData();
	PrimaryActorTick.bCanEverTick = true;

	SmokeShellRootScene = CreateDefaultSubobject<USceneComponent>(TEXT("SmokeShellRootScene"));
	SmokeShellMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SmokeShellMeshComponent"));
	SmokeShellHitCollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SmokeShellHitCollisionComponent"));
	UnDetectCollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("UnDectCollisionComponent"));
	ThrowFXComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ThrowFXComponent"));
	ExplosionFXComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ExplosionFXComponent"));
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));

	SmokeShellRootScene->SetupAttachment(RootComponent);

	SmokeShellMeshComponent->SetupAttachment(SmokeShellRootScene);
	SmokeShellMeshComponent->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);

	SmokeShellHitCollisionComponent->SetupAttachment(SmokeShellMeshComponent);
	SmokeShellHitCollisionComponent->SetCollisionProfileName(TEXT("PlayerHit"));
	SmokeShellHitCollisionComponent->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	SmokeShellHitCollisionComponent->SetSphereRadius(1.f);
	SmokeShellHitCollisionComponent->OnComponentBeginOverlap.AddDynamic(this,&ASmokeShell::DefaultCollisionBeginOverlap);

	UnDetectCollisionComponent->SetupAttachment(RootComponent);
	UnDetectCollisionComponent->SetCollisionProfileName(TEXT("InteractionRange"));
	UnDetectCollisionComponent->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	UnDetectCollisionComponent->OnComponentBeginOverlap.AddDynamic(this,&ASmokeShell::UnDetectCollisionBeginOverlap);
	UnDetectCollisionComponent->OnComponentEndOverlap.AddDynamic(this,&ASmokeShell::UnDetectCollisionEndOverlap);
	
	ProjectileMovementComponent->SetUpdatedComponent(RootComponent);
	ProjectileMovementComponent->bAutoActivate = false;
	ProjectileMovementComponent->Deactivate();

	ExplosionFXComponent->SetupAttachment(SmokeShellRootScene);
	ExplosionFXComponent->bAutoActivate = false;

	BodyHitComponent = SmokeShellHitCollisionComponent;
	RangeHitComponent = UnDetectCollisionComponent;
}

void ASmokeShell::BeginPlay()
{
	Super::BeginPlay();
}

void ASmokeShell::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASmokeShell::Enable_Implementation()
{
	Super::Enable_Implementation();
}

void ASmokeShell::Disable_Implementation()
{
	Super::Disable_Implementation();
	ResetDefaultState();
}

void ASmokeShell::Pushed_Implementation()
{
	Super::Pushed_Implementation();
}

void ASmokeShell::Obtained_Implementation()
{
	Super::Obtained_Implementation();
}

void ASmokeShell::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if(UWorld* LocWorld = UGlobalUtilsLibrary::GetValidWorld(this))
	{
		LocWorld->GetTimerManager().ClearTimer(UnDetectTimerHandle);
		LocWorld->GetTimerManager().ClearTimer(RotatorTimerHandle);
	}	
	Super::EndPlay(EndPlayReason);
}

void ASmokeShell::StartSkill()
{
	if(bAimState) ReleaseSkill();
}

void ASmokeShell::AimSkill()
{
	bCanceled = false;
	bAimState = true;
	AttachToActor(SkillOwner, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	ACombatCharacter* LocPlayerCharacter = Cast<ACorePlayerCharacter>(SkillOwner);
	SmokeShellMeshComponent->AttachToComponent(LocPlayerCharacter->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale,TEXT("hand_r"));
	SmokeShellMeshComponent->SetHiddenInGame(false);
	ThrowFXComponent->Activate(true);
}

void ASmokeShell::ActivateSkill()
{
	if(bActivated) return;
	bActivated = true;
	bAimState = false;
}

void ASmokeShell::ReleaseSkill()
{
	bCanceled = true;
	bReleased = true;
	FinishSkill();
}

void ASmokeShell::CancelSkill()
{
	bCanceled = true;
	FinishSkill();
}

void ASmokeShell::FinishSkill()
{
	Super::FinishSkill();
}

void ASmokeShell::SkillHitEnable(bool InbEnable)
{
	if (InbEnable)
	{
		ProjectileSmokeShell();
		SmokeShellHitCollisionComponent->SetCollisionEnabled(ECollisionEnabled::Type::QueryOnly);
		ACorePlayerCharacter* LocPlayerCharacter = Cast<ACorePlayerCharacter>(SkillOwner);
		LocPlayerCharacter->SetCurrentSkill(false);
	}
	else
	{
		ThrowFXComponent->Deactivate();
		ProjectileMovementComponent->Deactivate();
		SmokeShellHitCollisionComponent->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
		SmokeShellMeshComponent->SetHiddenInGame(true);
		UnDetectCollisionComponent->SetBoxExtent(FVector(SkillData.fEffectRange));
		UnDetectCollisionComponent->SetCollisionEnabled(ECollisionEnabled::Type::QueryOnly);
		if(UWorld* LocWorld = UGlobalUtilsLibrary::GetValidWorld(this))
		{
			LocWorld->GetTimerManager().SetTimer(UnDetectTimerHandle, this, &ASmokeShell::CheckActiveTime, LocWorld->DeltaTimeSeconds, true);
		}
	}
}

void ASmokeShell::ResetDefaultState()
{
	Super::ResetDefaultState();
	if(SkillOwner != nullptr) SkillOwner = nullptr;
	CurrentActiveTime = 0.f;
	bHit = false;
	bCanceled = false;
	bAimState = false;
	bActivated = false;
	bReleased = false;
	SmokeShellMeshComponent->SetHiddenInGame(true);
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	ThrowFXComponent->Deactivate();
	ExplosionFXComponent->Deactivate();
	if(CurrentOverlapPlayer) ReleaseEffects(CurrentOverlapPlayer);
	ReleaseNpc();
	UnDetectCollisionComponent->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
}

void ASmokeShell::ProjectileSmokeShell()
{
	if (SkillOwner)
	{
		DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		ThrowFXComponent->AttachToComponent(SmokeShellRootScene, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		SmokeShellMeshComponent->AttachToComponent(SmokeShellRootScene, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		SetParabolaLaunchVelocity();
		ProjectileMovementComponent->Velocity = LaunchVelocity;
		InitLocation = GetActorLocation();
		ProjectileMovementComponent->Activate();
		if(UWorld* LocWorld = UGlobalUtilsLibrary::GetValidWorld(this))
		{
			LocWorld->GetTimerManager().SetTimer(RotatorTimerHandle, this, &ASmokeShell::RotateSmokeMesh, LocWorld->DeltaTimeSeconds, true);
		}
	}
}

void ASmokeShell::ExplosionSmokeShell()
{
	MakeSkillNoise(true);
	SmokeShellSFXComponent = UGlobalUtilsLibrary::GetSharedSoundManager(this)->PlaySound3DAttached(TEXT("831019"), RootComponent, EAttachLocation::Type::SnapToTarget,TEXT("None"));
	if(UWorld* LocWorld = UGlobalUtilsLibrary::GetValidWorld(this)) LocWorld->GetTimerManager().ClearTimer(RotatorTimerHandle);
	ExplosionFXComponent->Activate(true);
	ExplosionFXComponent->SetFloatParameter(TEXT("Size"), SkillData.fEffectRange / 100.f);
	ExplosionFXComponent->SetIntParameter(TEXT("LoopCount"), SkillEffects[0].fEffectDuration);
	bHit = true;
	SkillHitEnable(false);
}

void ASmokeShell::CheckActiveTime()
{
	const UWorld* LocWorld = UGlobalUtilsLibrary::GetValidWorld(this);
	if(LocWorld == nullptr) return;
	if(CurrentActiveTime > SkillEffects[0].fEffectDuration)
	{
		LocWorld->GetTimerManager().ClearTimer(UnDetectTimerHandle);
		Super::FinishSkill();
	}
	else
	{
		CurrentActiveTime = CurrentActiveTime + LocWorld->DeltaTimeSeconds;
	}
}

void ASmokeShell::RotateSmokeMesh() const
{
	FRotator LocAddRotator(10.f,3.f,1.f);
	SmokeShellMeshComponent->AddLocalRotation(LocAddRotator);
}

void ASmokeShell::ReleaseEffects(AActor* InReleaseActor)
{
	TArray<FEffect> LocReleaseEffects;
	FEffect LocReleaseEffect;
	if(Cast<ACorePlayerCharacter>(InReleaseActor))
	{
		LocReleaseEffect.sEffect = EnumToString(EEffectSubTypes::CogImmune);
		LocReleaseEffects.Add(LocReleaseEffect);
	}
	if(Cast<ACoreNpc>(InReleaseActor))
	{
		LocReleaseEffect.sEffect = EnumToString(EEffectSubTypes::Unrecog);
		LocReleaseEffects.Add(LocReleaseEffect);
	}		
	IDamageable::Execute_ApplyRecoveryEvent(InReleaseActor, LocReleaseEffects);
}

void ASmokeShell::ReleaseNpc() const
{
	AWorldGameMode* LocGameMode = UGlobalUtilsLibrary::GetGameModeChecked<AWorldGameMode>(this);
	TArray<AActor*> LocNpcCharacters;
	LocGameMode->GetNpcCharacters(LocNpcCharacters);
	for(AActor* LocNpc : LocNpcCharacters)
	{
		TArray<EEffectSubTypes> LocNpcEffectTypes;
		IDamageable::Execute_GetCurrentEffectTypes(LocNpc, LocNpcEffectTypes);
		if(!LocNpcEffectTypes.Contains(EEffectSubTypes::Unrecog))
		{
			TArray<FEffect> LocEffects;
			FEffect LocEffect;
			const FString LocUnrecogString = UGlobalUtilsLibrary::GetEnumToString<EEffectSubTypes>(EEffectSubTypes::Unrecog);
			LocEffect.sEffect = LocUnrecogString;
			LocEffects.Add(LocEffect);
			IDamageable::Execute_ApplyRecoveryEvent(LocNpc, LocEffects);
		}
	}
}

void ASmokeShell::DefaultCollisionBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                               UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(!bHit && OtherComp->GetCollisionProfileName() == TEXT("Ground")) ExplosionSmokeShell();
}

void ASmokeShell::UnDetectCollisionBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(ACorePlayerCharacter* LocPlayer = Cast<ACorePlayerCharacter>(OtherActor))
	{
		CurrentOverlapPlayer = LocPlayer;
		IDamageable::Execute_ApplyDamageEvent(OtherActor, Cast<ACombatCharacter>(SkillOwner), GetActorLocation(), SkillData.fDamage, SkillEffects);
	}
	if(ACoreNpc* LocNpc = Cast<ACoreNpc>(OtherActor))
	{
		CurrentOverlapNpc.AddUnique(LocNpc);
		IDamageable::Execute_ApplyDamageEvent(LocNpc, Cast<ACombatCharacter>(SkillOwner), GetActorLocation(), SkillData.fDamage, SkillEffects);
	}
}

void ASmokeShell::UnDetectCollisionEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if(ACorePlayerCharacter* LocPlayer = Cast<ACorePlayerCharacter>(OtherActor))
	{
		CurrentOverlapPlayer = nullptr;
		ReleaseEffects(LocPlayer);
	}
	if(ACoreNpc* LocNpc = Cast<ACoreNpc>(OtherActor))
	{
		CurrentOverlapNpc.Remove(LocNpc);
		ReleaseEffects(LocNpc);
	}
}
