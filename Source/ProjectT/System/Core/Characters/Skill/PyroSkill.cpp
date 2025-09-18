// Fill out your copyright notice in the Description page of Project Settings.


#include "PyroSkill.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/AudioComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "ProjectT/System/Core/Characters/Player/CorePlayerCharacter.h"
#include "ProjectT/System/Core/Characters/NPC/CoreNpc.h"
#include "ProjectT/System/Core/Common/GlobalUtilsLibrary.h"
#include "ProjectT/System/Core/Managers/SoundManager.h"

APyroSkill::APyroSkill() :
	FireBallRootScene(nullptr),
	ThrowFXComponent(nullptr),
	ExplosionFXComponent(nullptr),
	FireBallMeshComponent(nullptr),
	FireBallHitCollisionComponent(nullptr),
	FireBallEffectCollisionComponent(nullptr),
	ThrowSFXComponent(nullptr),
	ExplosionSFXComponent(nullptr),
	ProjectileMovementComponent(nullptr),
	bHit(false)
{
	SkillPoolsize = 5;
	SkillRowName = "312201";
	SkillId = 312201;
	InitializeSkillData();
	PrimaryActorTick.bCanEverTick = true;

	FireBallRootScene = CreateDefaultSubobject<USceneComponent>(TEXT("FireBallRootScene"));
	FireBallMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FireBallMeshComponent"));
	FireBallHitCollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("FireBallHitCollisionComponent"));
	FireBallEffectCollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("FireBallEffectCollisionComponent"));
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ThrowFXComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ThrowFxComponent"));
	ExplosionFXComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ExplosionFxComponent"));
				
	FireBallRootScene->SetupAttachment(RootComponent);

	FireBallMeshComponent->SetupAttachment(FireBallRootScene);
	FireBallMeshComponent->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	FireBallMeshComponent->SetHiddenInGame(true);

	FireBallHitCollisionComponent->SetupAttachment(FireBallMeshComponent);
	FireBallHitCollisionComponent->SetCollisionProfileName(TEXT("PlayerHit"));
	FireBallHitCollisionComponent->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	FireBallHitCollisionComponent->OnComponentBeginOverlap.AddDynamic(this,&APyroSkill::DefaultCollisionBeginOverlap);
	
	FireBallEffectCollisionComponent->SetupAttachment(RootComponent);
	FireBallEffectCollisionComponent->SetCollisionProfileName(TEXT("PlayerHit"));
	FireBallEffectCollisionComponent->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	FireBallEffectCollisionComponent->SetSphereRadius(SkillData.fEffectRange);
			
	ProjectileMovementComponent->SetUpdatedComponent(RootComponent);
	ProjectileMovementComponent->InitialSpeed = ProjectileMovementComponent->MaxSpeed = SkillData.fProjectileSpeed;
	ProjectileMovementComponent->ProjectileGravityScale = 0.f;
	ProjectileMovementComponent->bAutoActivate = false;
	ProjectileMovementComponent->Deactivate();

	ThrowFXComponent->SetupAttachment(FireBallRootScene);
	ThrowFXComponent->bAutoActivate = false;
	ExplosionFXComponent->SetupAttachment(FireBallRootScene);
	ExplosionFXComponent->bAutoActivate = false;

	BodyHitComponent = FireBallHitCollisionComponent;
	RangeHitComponent = FireBallEffectCollisionComponent;
}

void APyroSkill::BeginPlay()
{
	Super::BeginPlay();
	SetActorTickEnabled(false);	
}

void APyroSkill::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APyroSkill::Enable_Implementation()
{
	Super::Enable_Implementation();
}

void APyroSkill::Disable_Implementation()
{
	Super::Disable_Implementation();
	ResetDefaultState();
}

void APyroSkill::Pushed_Implementation()
{
	Super::Pushed_Implementation();
}

void APyroSkill::Obtained_Implementation()
{
	Super::Obtained_Implementation();
}

void APyroSkill::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if(UWorld* LocWorld = UGlobalUtilsLibrary::GetValidWorld(this))
	{
		LocWorld->GetTimerManager().ClearTimer(RangeCheckTimer);
		LocWorld->GetTimerManager().ClearTimer(ExplosionCheckTimer);
	}
	Super::EndPlay(EndPlayReason);
}

void APyroSkill::StartSkill()
{
	if(bAimState) ReleaseSkill(); 
}

void APyroSkill::AimSkill()
{
	ACorePlayerCharacter* LocPlayerCharacter = Cast<ACorePlayerCharacter>(SkillOwner);
	if(!LocPlayerCharacter) return;
	bCanceled = false;
	bAimState = true;
	AttachToActor(LocPlayerCharacter,FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	ThrowFXComponent->AttachToComponent(LocPlayerCharacter->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale,TEXT("SKT_Pyro_Fireball_R"));
	ThrowFXComponent->Activate(true);
}

void APyroSkill::ActivateSkill()
{
	if(bActivated) return;
	bAimState = false;
	bActivated = true;
}

void APyroSkill::ReleaseSkill()
{
	bCanceled = true;
	bReleased = true;
	FinishSkill();
}

void APyroSkill::CancelSkill()
{
	bCanceled = true;
	FinishSkill();
}

void APyroSkill::FinishSkill()
{
	Super::FinishSkill();
}

void APyroSkill::SkillHitEnable(bool InbEnable)
{
	if(InbEnable)
	{
		FireBallMeshComponent->SetHiddenInGame(false);
		ProjectileFireBall();
		FireBallHitCollisionComponent->SetCollisionEnabled(ECollisionEnabled::Type::QueryOnly);
		NMT_CHECKF(SkillOwner);
		if(ACorePlayerCharacter* LocPlayerCharacter = Cast<ACorePlayerCharacter>(SkillOwner))
		LocPlayerCharacter->SetCurrentSkill(false);
	}
	else
	{
		UWorld* LocWorld = UGlobalUtilsLibrary::GetValidWorld(this);
		if(!LocWorld) return;
		FireBallEffectCollisionComponent->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
		ThrowFXComponent->Deactivate();
		FireBallMeshComponent->SetHiddenInGame(true);
		LocWorld->GetTimerManager().ClearTimer(RangeCheckTimer);
		RangeCheckTimer.Invalidate();
		LocWorld->GetTimerManager().SetTimer(ExplosionCheckTimer, this, &APyroSkill::CheckActiveExplosion, LocWorld->DeltaTimeSeconds, true);
	}
}

void APyroSkill::ResetDefaultState()
{
	Super::ResetDefaultState();
	if (SkillOwner != nullptr)
	{
		SkillOwner = nullptr;
	}
	bHit = false;
	bCanceled = false;
	bAimState = false;
	bActivated = false;
	bReleased = false;
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	ThrowFXComponent->Deactivate();
}

void APyroSkill::ProjectileFireBall()
{
	if (SkillOwner)
	{
		ThrowSFXComponent = UGlobalUtilsLibrary::GetSharedSoundManager(this)->PlaySound3DAttached(TEXT("831002"), RootComponent, EAttachLocation::Type::SnapToTarget,TEXT("None"));
		ACorePlayerCharacter* LocPlayerCharacter = Cast<ACorePlayerCharacter>(SkillOwner);
		ThrowFXComponent->AttachToComponent(FireBallRootScene, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		AttachToComponent(LocPlayerCharacter->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale,TEXT("SKT_Pyro_Fireball_R"));
		DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		FVector LocMaxLocation = LocPlayerCharacter->SkillRootScene->GetComponentLocation() +
			UKismetMathLibrary::GetForwardVector(LocPlayerCharacter->SkillRootScene->GetComponentRotation()) * SkillData.fSkillRange;
		FRotator LocLookAtMaxLocation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(),LocMaxLocation);
		SetActorRotation(LocLookAtMaxLocation);
		ProjectileMovementComponent->Velocity = GetActorForwardVector() * SkillData.fProjectileSpeed;
		ProjectileMovementComponent->Activate();
		InitLocation = GetActorLocation();
		if(UWorld* LocWorld = UGlobalUtilsLibrary::GetValidWorld(this))
		{
			LocWorld->GetTimerManager().SetTimer(RangeCheckTimer, this, &APyroSkill::CheckDistance, LocWorld->DeltaTimeSeconds, true);	
		}		
	}
}

void APyroSkill::ExplosionFireBall()
{
	bHit = true;
	if(ThrowSFXComponent) ThrowSFXComponent.Get()->StopDelayed(0.15f);
	UGlobalUtilsLibrary::GetSharedSoundManager(this)->PlaySound3DAttached(TEXT("831003"), RootComponent, EAttachLocation::Type::SnapToTarget,TEXT("None"));
	ProjectileMovementComponent->Deactivate();
	FireBallHitCollisionComponent->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	FireBallEffectCollisionComponent->SetCollisionEnabled(ECollisionEnabled::Type::QueryOnly);
	TArray<AActor*> OverlappedActors;
	FireBallEffectCollisionComponent->GetOverlappingActors(OverlappedActors, ACoreNpc::StaticClass());
	ExplosionFXComponent->Activate(true);
	if (OverlappedActors.Num() >= 1)
	{
		for (AActor* LocOverlapActor : OverlappedActors)
		{
			TArray<AActor*> LocIgnoreActors;
			FHitResult LocTraceHitResult;
			UKismetSystemLibrary::LineTraceSingle(this, GetActorLocation(), LocOverlapActor->GetActorLocation(),
												   LinearTraceQuery, false, LocIgnoreActors, EDrawDebugTrace::None, LocTraceHitResult, true);
			if(!LocTraceHitResult.bBlockingHit)
			{
				IDamageable::Execute_ApplyDamageEvent(LocOverlapActor, Cast<ACombatCharacter>(SkillOwner), GetActorLocation(), GetFinalDamage(LocOverlapActor, true), SkillEffects);
				if(UWorld* LocWorld = UGlobalUtilsLibrary::GetValidWorld(this))
				{
					UNiagaraFunctionLibrary::SpawnSystemAtLocation(LocWorld, HitFXSystem, LocOverlapActor->GetActorLocation());	
				}				
			}			
		}
	}
	MakeSkillNoise(true);
	SkillHitEnable(false);
}

void APyroSkill::CheckDistance()
{
	if(UKismetMathLibrary::Vector_Distance(InitLocation, GetActorLocation()) > SkillData.fSkillRange)
	{
		ExplosionFireBall();
	}
}

void APyroSkill::CheckActiveExplosion()
{
	if(ExplosionFXComponent->IsActive()) return;
	if(UWorld* LocWorld = UGlobalUtilsLibrary::GetValidWorld(this))
	{
		LocWorld->GetTimerManager().ClearTimer(ExplosionCheckTimer);
		ExplosionCheckTimer.Invalidate();
		FinishSkill();	
	}	
}

void APyroSkill::DefaultCollisionBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                              UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(!bHit && OtherActor != SkillOwner)
	{
		MovedComponentToHitPoint(OtherActor, FireBallEffectCollisionComponent);
		ExplosionFireBall();
	}
}

