// Fill out your copyright notice in the Description page of Project Settings.


#include "NoxAtk.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "ProjectT/System/Core/Characters/NPC/CoreNpc.h"
#include "ProjectT/System/Core/Characters/Player/CorePlayerCharacter.h"
#include "ProjectT/System/Core/Common/GlobalUtilsLibrary.h"

ANoxAtk::ANoxAtk() :
	NoxAtkRootSceneComponent(nullptr),
	NoxAtkHitCollisionComponent(nullptr),
	NoxAtkEffectCollisionComponent(nullptr),
	ThrowFXComponent(nullptr),
	WandFXComponent(nullptr),
	BindFXAsset(nullptr),
	ProjectileMovementComponent(nullptr),
	bHit(false),
	BindTime(0.f)
{
	SkillPoolsize = 5;
	SkillRowName = "311204";
	SkillId = 311204;
	InitializeSkillData();
	PrimaryActorTick.bCanEverTick = true;

	NoxAtkRootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("NoxAtkRootScene"));
	NoxAtkHitCollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("NoxAtkCollisionComponent"));
	NoxAtkEffectCollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("NoxAtkEffectCollisionComponent"));
	ThrowFXComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ThrowFXComponent"));
	WandFXComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("WandFXComponent"));
	ProjectileMovementComponent= CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));

	NoxAtkRootSceneComponent->SetupAttachment(RootComponent);
	
	NoxAtkHitCollisionComponent->SetupAttachment(RootComponent);
	NoxAtkHitCollisionComponent->SetCollisionProfileName(TEXT("PlayerHit"));
	NoxAtkHitCollisionComponent->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	NoxAtkHitCollisionComponent->OnComponentBeginOverlap.AddDynamic(this,&ANoxAtk::DefaultCollisionBeginOverlap);

	NoxAtkEffectCollisionComponent->SetupAttachment(RootComponent);
	NoxAtkEffectCollisionComponent->SetCollisionProfileName(TEXT("PlayerHit"));
	NoxAtkEffectCollisionComponent->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	NoxAtkEffectCollisionComponent->SetSphereRadius(SkillData.fEffectRange);

	ThrowFXComponent->SetupAttachment(RootComponent);
	ThrowFXComponent->bAutoActivate = false;
	WandFXComponent->SetupAttachment(RootComponent);
	WandFXComponent->bAutoActivate = false;
	
	ProjectileMovementComponent->SetUpdatedComponent(RootComponent);
	ProjectileMovementComponent->InitialSpeed = ProjectileMovementComponent->MaxSpeed = SkillData.fProjectileSpeed;
	ProjectileMovementComponent->ProjectileGravityScale = 0.f;
	ProjectileMovementComponent->bAutoActivate = false;
	ProjectileMovementComponent->Deactivate();

	BodyHitComponent = NoxAtkHitCollisionComponent;
	RangeHitComponent = NoxAtkHitCollisionComponent;
}

void ANoxAtk::BeginPlay()
{
	Super::BeginPlay();	
}

void ANoxAtk::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ANoxAtk::Enable_Implementation()
{
	Super::Enable_Implementation();
}

void ANoxAtk::Disable_Implementation()
{
	Super::Disable_Implementation();
	ResetDefaultState();
}

void ANoxAtk::Pushed_Implementation()
{
	Super::Pushed_Implementation();
}

void ANoxAtk::Obtained_Implementation()
{
	Super::Obtained_Implementation();
}

void ANoxAtk::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if(UWorld* LocWorld = UGlobalUtilsLibrary::GetValidWorld(this))
	{
		LocWorld->GetTimerManager().ClearTimer(RangeCheckTimer);
		LocWorld->GetTimerManager().ClearTimer(BindEffectTimer);
		LocWorld->GetTimerManager().ClearTimer(BindCheckTimer);
	}
	Super::EndPlay(EndPlayReason);
}

void ANoxAtk::StartSkill()
{
	ACorePlayerCharacter* LocPlayerCharacter = Cast<ACorePlayerCharacter>(SkillOwner);
	if(!LocPlayerCharacter) return;
	bCanceled = false;
	TObjectPtr<UStaticMeshComponent> LocNoxWandMesh;
	LocPlayerCharacter->GetWeaponStaticComponent(LocNoxWandMesh);
	AttachToComponent(LocNoxWandMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("SKT_Gem"));
}

void ANoxAtk::CancelSkill()
{
	SkillHitEnable(false);
	SkillFXEnable(false, 0);
	Super::CancelSkill();
}

void ANoxAtk::FinishSkill()
{
	Super::FinishSkill();
}

void ANoxAtk::SkillHitEnable(bool InbEnable)
{
	if(InbEnable)
	{
		ProjectileNoxBall();
	}
	else
	{
		ThrowFXComponent->Deactivate();
		ProjectileMovementComponent->Deactivate();
		NoxAtkEffectCollisionComponent->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
		if(UWorld* LocWorld = UGlobalUtilsLibrary::GetValidWorld(this))
		{			
		LocWorld->GetTimerManager().ClearTimer(RangeCheckTimer);
		LocWorld->GetTimerManager().SetTimer(BindCheckTimer, this, &ANoxAtk::CheckActiveBind, LocWorld->DeltaTimeSeconds, true);
		}
	}
}

void ANoxAtk::SkillFXEnable(bool InbEnable, int32 InIndex)
{
	if(InbEnable)
	{
		WandFXComponent->Activate(true);
	}
	else
	{
		WandFXComponent->Deactivate();
	}
}

void ANoxAtk::ResetDefaultState()
{
	if(ProjectileMovementComponent->IsActive()) return;
	Super::ResetDefaultState();
	bHit = false;
	bCanceled = false;
	bAimState = false;
	bActivated = false;
	bReleased = false;
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	ThrowFXComponent->Deactivate();
}

void ANoxAtk::ProjectileNoxBall()
{
	if (SkillOwner)
	{
		ACorePlayerCharacter* LocPlayerCharacter = Cast<ACorePlayerCharacter>(SkillOwner);
		if(!LocPlayerCharacter) return;
		DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		ThrowFXComponent->Activate(true);
		ThrowFXComponent->SetFloatParameter(TEXT("All Rate"), 1.f);
		FVector LocMaxLocation = LocPlayerCharacter->SkillRootScene->GetComponentLocation() +
			UKismetMathLibrary::GetForwardVector(LocPlayerCharacter->SkillRootScene->GetComponentRotation()) * SkillData.fSkillRange;
		FRotator LocLookAtMaxLocation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(),LocMaxLocation);
		SetActorRotation(LocLookAtMaxLocation);
		ProjectileMovementComponent->Velocity = GetActorForwardVector() * SkillData.fProjectileSpeed;
		ProjectileMovementComponent->Activate();
		NoxAtkHitCollisionComponent->SetCollisionEnabled(ECollisionEnabled::Type::QueryOnly);
		InitLocation = GetActorLocation();
		if(UWorld* LocWorld = UGlobalUtilsLibrary::GetValidWorld(this))
		{
			LocWorld->GetTimerManager().SetTimer(RangeCheckTimer, this, &ANoxAtk::CheckDistance, LocWorld->DeltaTimeSeconds, true);
		}
	}
}

void ANoxAtk::ExplosionNoxBall()
{
	bHit = true;
	NoxAtkHitCollisionComponent->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	ThrowFXComponent->Deactivate();
	ThrowFXComponent->SetFloatParameter(TEXT("All Rate"), 0.f);
	NoxAtkEffectCollisionComponent->SetCollisionEnabled(ECollisionEnabled::Type::QueryOnly);
	TArray<AActor*> OverlappedActors;
	NoxAtkEffectCollisionComponent->GetOverlappingActors(OverlappedActors, ACoreNpc::StaticClass());
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
				if(UWorld* LocWorld = UGlobalUtilsLibrary::GetValidWorld(this)) UNiagaraFunctionLibrary::SpawnSystemAtLocation(LocWorld, HitFXSystem, LocOverlapActor->GetActorLocation());
				USkeletalMeshComponent* LocMesh = Cast<ACombatCharacter>(LocOverlapActor)->GetMesh();
				UNiagaraFunctionLibrary::SpawnSystemAttached(BindFXAsset, LocMesh, FName("None"),FVector::Zero(),FRotator::ZeroRotator,EAttachLocation::Type::SnapToTarget,true,true);
			}			
		}
	}
	MakeSkillNoise(true);
	SkillHitEnable(false);
}

void ANoxAtk::CheckDistance()
{
	if(UKismetMathLibrary::Vector_Distance(InitLocation, GetActorLocation()) > SkillData.fSkillRange)
	{
		ExplosionNoxBall();
	}
}

void ANoxAtk::CheckActiveBind()
{
	if(UWorld* LocWorld = UGlobalUtilsLibrary::GetValidWorld(this))
	{
		BindTime = BindTime + LocWorld->DeltaTimeSeconds;
		if(BindTime < SkillEffects[0].fEffectDuration) return;
		BindTime = 0.f;
		LocWorld->GetTimerManager().ClearTimer(BindCheckTimer);
		FinishSkill();
	}
}

void ANoxAtk::DefaultCollisionBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(!bHit && OtherActor != SkillOwner)
	{
		MovedComponentToHitPoint(OtherActor, NoxAtkEffectCollisionComponent);
		ExplosionNoxBall();
	}	
}

