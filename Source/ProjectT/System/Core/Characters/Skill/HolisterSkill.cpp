// Fill out your copyright notice in the Description page of Project Settings.


#include "HolisterSkill.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "ProjectT/System/Core/Characters/NPC/CoreNpc.h"
#include "ProjectT/System/Core/Characters/Player/CorePlayerCharacter.h"
#include "ProjectT/System/Core/Common/GlobalUtilsLibrary.h"

AHolisterSkill::AHolisterSkill() :
	HolisterSkillRootSceneComponent(nullptr),
	HolisterSkillCollisionComponent(nullptr),
	HolisterSkillFXComponent(nullptr)
{
	SkillPoolsize = 3;
	SkillRowName = "312103";
	SkillId = 312103;
	InitializeSkillData();
	PrimaryActorTick.bCanEverTick = true;

	HolisterSkillRootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("HolisterAtkRootSceneComponent"));
	HolisterSkillCollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("HolisterAtkCollisionComponent"));
	HolisterSkillFXComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("HolisterAtkFXComponent"));

	HolisterSkillRootSceneComponent->SetupAttachment(RootComponent);
	HolisterSkillCollisionComponent->SetupAttachment(HolisterSkillRootSceneComponent);
	HolisterSkillCollisionComponent->SetCollisionProfileName(TEXT("PlayerHit"));
	HolisterSkillCollisionComponent->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	HolisterSkillCollisionComponent->OnComponentBeginOverlap.AddDynamic(this,&AHolisterSkill::BoxCollisionBeginOverlap);
	
	HolisterSkillFXComponent->SetupAttachment(RootComponent);
	HolisterSkillFXComponent->bAutoActivate = false;
	
	BodyHitComponent = HolisterSkillCollisionComponent;
	RangeHitComponent = HolisterSkillCollisionComponent;
}

void AHolisterSkill::BeginPlay()
{
	Super::BeginPlay();
	
}

void AHolisterSkill::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AHolisterSkill::Enable_Implementation()
{
	Super::Enable_Implementation();
}

void AHolisterSkill::Disable_Implementation()
{
	Super::Disable_Implementation();
	if (SkillOwner == nullptr) return;
	SkillOwner = nullptr;
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
}

void AHolisterSkill::Pushed_Implementation()
{
	Super::Pushed_Implementation();
}

void AHolisterSkill::Obtained_Implementation()
{
	Super::Obtained_Implementation();
}

void AHolisterSkill::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void AHolisterSkill::StartSkill()
{
	Super::StartSkill();
	if(ACorePlayerCharacter* LocPlayerCharacter = Cast<ACorePlayerCharacter>(SkillOwner))
	{
		AttachToComponent(LocPlayerCharacter->SkillRootScene, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		TObjectPtr<UStaticMeshComponent> LocHolisterShotgunMesh;
		LocPlayerCharacter->GetWeaponStaticComponent(LocHolisterShotgunMesh);
		HolisterSkillFXComponent->AttachToComponent(LocHolisterShotgunMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("SKT_Shoot"));
	}
}

void AHolisterSkill::CancelSkill()
{
	SkillHitEnable(false);
	SkillFXEnable(false);
	Super::CancelSkill();
}

void AHolisterSkill::FinishSkill()
{
	Super::FinishSkill();
}

void AHolisterSkill::SkillHitEnable(bool InbEnable)
{
	if(InbEnable)
	{
		HolisterSkillCollisionComponent->SetCollisionEnabled(ECollisionEnabled::Type::QueryOnly);
		MakeSkillNoise();
		HolisterSkillCollisionComponent->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	}
	else
	{
		HolisterSkillCollisionComponent->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	}
}

void AHolisterSkill::SkillFXEnable(bool InbEnable, int32 InIndex)
{
	if(!HolisterSkillFXComponent->GetAsset()) return;
	if(InbEnable)
	{
		HolisterSkillFXComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		HolisterSkillFXComponent->Activate(true);
	}
	else
	{
		HolisterSkillFXComponent->Deactivate();
	}
}

void AHolisterSkill::BoxCollisionBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	const FHitResult& SweepResult)
{
	if(ACoreNpc* LocHitNpc = Cast<ACoreNpc>(OtherActor))
	{
		TArray<AActor*> LocIgnoreActors;
		FHitResult LocTraceHitResult;
		UKismetSystemLibrary::LineTraceSingle(this, GetActorLocation(), LocHitNpc->GetActorLocation(),
											   LinearTraceQuery, false, LocIgnoreActors, EDrawDebugTrace::None, LocTraceHitResult, true);
		if(!LocTraceHitResult.bBlockingHit)
		{
			IDamageable::Execute_ApplyDamageEvent(LocHitNpc, Cast<ACombatCharacter>(SkillOwner), GetActorLocation(), GetFinalDamage(LocHitNpc), SkillEffects);
			if(UWorld* LocWorld = UGlobalUtilsLibrary::GetValidWorld(this)) UNiagaraFunctionLibrary::SpawnSystemAtLocation(LocWorld, HitFXSystem, LocHitNpc->GetActorLocation());	
		}		
	}
}

