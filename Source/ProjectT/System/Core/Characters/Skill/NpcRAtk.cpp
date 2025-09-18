// Fill out your copyright notice in the Description page of Project Settings.


#include "NpcRAtk.h"

#include "NiagaraComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "ProjectT/System/Core/Characters/Player/CorePlayerCharacter.h"
#include "ProjectT/System/Core/Common/GlobalUtilsLibrary.h"
#include "ProjectT/System/SinglePlay/Characters/NPC/SPNpcAgent.h"


ANpcRAtk::ANpcRAtk() :
	DirectAttackCollisionComponent(nullptr),
	BulletMeshComponent(nullptr),
	ShootFXComponent(nullptr),
	BulletTrailFXComponent(nullptr),
	BulletHitFXComponent(nullptr),
	ProjectileMovementComponent(nullptr),
	SkillNpcOwner(nullptr),
	StartLocation(FVector::ZeroVector),
	bHit(false)
{
	SkillRowName = "311251";
	SkillId = 311251;
	SkillPoolsize = 10;
	InitializeSkillData();
	PrimaryActorTick.bCanEverTick = true;

	DirectAttackCollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("DirectAttackCollisionComponent"));
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	BulletMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BulletMeshComponent"));
	ShootFXComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ShootFXComponent"));
	BulletHitFXComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("BulletHitFXComponent"));
	BulletTrailFXComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("BulletTrailFXComponent"));
	NMT_CHECKF(DirectAttackCollisionComponent && ProjectileMovementComponent && BulletMeshComponent && ShootFXComponent && BulletHitFXComponent && BulletTrailFXComponent)
	
	DirectAttackCollisionComponent->SetSphereRadius(16.f);
	DirectAttackCollisionComponent->SetCollisionProfileName(TEXT("EnemyHit"));
	DirectAttackCollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	DirectAttackCollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &ANpcRAtk::DefaultCollisionBeginOverlap);
	DirectAttackCollisionComponent->SetupAttachment(RootComponent);
	DirectAttackCollisionComponent->SetUseCCD(true);

	ProjectileMovementComponent->SetUpdatedComponent(RootComponent);
	ProjectileMovementComponent->InitialSpeed = ProjectileMovementComponent->MaxSpeed = SkillData.fProjectileSpeed;
	ProjectileMovementComponent->ProjectileGravityScale = 0.f;
	ProjectileMovementComponent->bAutoActivate = false;
	ProjectileMovementComponent->Deactivate();

	BulletMeshComponent->SetupAttachment(RootComponent);
	BulletMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BulletMeshComponent->SetHiddenInGame(true);

	ShootFXComponent->SetupAttachment(RootComponent);
	ShootFXComponent->bAutoActivate = false;

	BulletHitFXComponent->SetupAttachment(RootComponent);
	BulletHitFXComponent->bAutoActivate = false;

	BulletTrailFXComponent->SetupAttachment(RootComponent);
	BulletTrailFXComponent->bAutoActivate = false;

	BodyHitComponent = DirectAttackCollisionComponent;
	RangeHitComponent = DirectAttackCollisionComponent;
}

void ANpcRAtk::BeginPlay()
{
	Super::BeginPlay();

	SetActorTickEnabled(false);
}

void ANpcRAtk::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ANpcRAtk::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	UWorld* World = UGlobalUtilsLibrary::GetValidWorld(this);
	World->GetTimerManager().ClearTimer(RangeCheckTimerHandle);
	World->GetTimerManager().ClearTimer(ShootPoseCoolDownTimerHandle);
}

void ANpcRAtk::Enable_Implementation()
{
	Super::Enable_Implementation();
}

void ANpcRAtk::Disable_Implementation()
{
	Super::Disable_Implementation();

	ResetDefaultState();
}

void ANpcRAtk::Pushed_Implementation()
{
	Super::Pushed_Implementation();
}

void ANpcRAtk::Obtained_Implementation()
{
	Super::Obtained_Implementation();
}

void ANpcRAtk::SkillFXEnable(bool InbEnable, int32 InIndex)
{
	if(InbEnable)
	{
		ShootFXComponent->Activate(true);
	}
	else
	{
		ShootFXComponent->Deactivate();
	}
}

void ANpcRAtk::ResetDefaultState()
{
	Super::ResetDefaultState();

	SetActorTickEnabled(false);
	SetSkillOwner(nullptr);
	SkillHitEnable(false);
	SkillFXEnable(false);
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	SetActorLocation(FVector::ZeroVector);
	bHit = false;
}

void ANpcRAtk::StartSkill()
{
	Super::StartSkill();
}

void ANpcRAtk::ActivateSkill()
{
	Super::ActivateSkill();

	SkillNpcOwner = Cast<ASPNpcAgent>(SkillOwner);
	if(SkillNpcOwner)
	{
		AttachToComponent(SkillNpcOwner->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("SKT_NPC_Gun_R"));
		ShootFXComponent->AttachToComponent(SkillNpcOwner->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("SKT_NPC_Gun_R"));
	}
}

void ANpcRAtk::FinishSkill()
{
	SkillHitEnable(false);
	SetActorTickEnabled(false);
	
	Super::FinishSkill();
}

void ANpcRAtk::SkillHitEnable(bool InbEnable)
{
	if(InbEnable)
	{
		DirectAttackCollisionComponent->SetCollisionEnabled(ECollisionEnabled::Type::QueryOnly);
		BulletMeshComponent->SetHiddenInGame(false);
		ShootProjectile();
	}
	else
	{
		DirectAttackCollisionComponent->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
		BulletMeshComponent->SetHiddenInGame(true);

		GetWorldTimerManager().ClearTimer(RangeCheckTimerHandle);
		RangeCheckTimerHandle.Invalidate();

		ProjectileMovementComponent->Deactivate();
		BulletTrailFXComponent->Deactivate();
	}
}

void ANpcRAtk::DefaultCollisionBeginOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                            const FHitResult& SweepResult)
{
	if(OtherActor != SkillNpcOwner && !Cast<ACoreNpc>(OtherActor))
	{
		if(Cast<ACorePlayerCharacter>(OtherActor))
		{
			IDamageable::Execute_ApplyDamageEvent(OtherActor, Cast<ACombatCharacter>(SkillOwner), GetActorLocation(), SkillData.fDamage, SkillEffects);
		}
		bHit = true;
		BulletHitFXComponent->Activate(true);
		SkillHitEnable(false);
	}
}

void ANpcRAtk::ShootProjectile()
{
	if(SkillNpcOwner)
	{
		DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		ProjectileMovementComponent->Velocity = SkillNpcOwner->GetActorForwardVector() * SkillData.fProjectileSpeed;
		ProjectileMovementComponent->Activate();
		StartLocation = GetActorLocation();
		BulletTrailFXComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		BulletTrailFXComponent->Activate(true);
		UWorld* World = UGlobalUtilsLibrary::GetValidWorld(this);
		World->GetTimerManager().SetTimer(RangeCheckTimerHandle, this, &ANpcRAtk::CheckDistance, World->GetDeltaSeconds(), true);
		if(bHit)
		{
			SkillHitEnable(false);
		}
	}
}

void ANpcRAtk::CheckDistance()
{
	if(UKismetMathLibrary::Vector_Distance(StartLocation, GetActorLocation()) > SkillData.fSkillRange)
	{
		SkillHitEnable(false);
	}
}
