// Fill out your copyright notice in the Description page of Project Settings.


#include "HolisterAtk.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "ProjectT/System/Core/Characters/NPC/CoreNpc.h"
#include "ProjectT/System/Core/Characters/Player/CorePlayerCharacter.h"
#include "ProjectT/System/Core/Common/GlobalUtilsLibrary.h"

AHolisterAtk::AHolisterAtk() :
	HolisterAtkRootSceneComponent(nullptr),
	HolisterAtkCollisionComponent(nullptr),
	HolisterAtkFXComponent(nullptr)
{

	SkillPoolsize = 5;
	SkillRowName = "311103";
	SkillId = 311103;
	InitializeSkillData();
	PrimaryActorTick.bCanEverTick = true;

	HolisterAtkRootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("HolisterAtkRootSceneComponent"));
	HolisterAtkCollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("HolisterAtkCollisionComponent"));
	HolisterAtkFXComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("HolisterAtkFXComponent"));
	
	HolisterAtkRootSceneComponent->SetupAttachment(RootComponent);
	HolisterAtkCollisionComponent->SetupAttachment(HolisterAtkRootSceneComponent);
	HolisterAtkCollisionComponent->SetCollisionProfileName(TEXT("PlayerHit"));
	HolisterAtkCollisionComponent->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	HolisterAtkCollisionComponent->OnComponentBeginOverlap.AddDynamic(this,&AHolisterAtk::BoxCollisionBeginOverlap);
	
	HolisterAtkFXComponent->SetupAttachment(RootComponent);
	HolisterAtkFXComponent->bAutoActivate = false;
	
	BodyHitComponent = HolisterAtkCollisionComponent;
	RangeHitComponent = HolisterAtkCollisionComponent;
}

void AHolisterAtk::BeginPlay()
{
	Super::BeginPlay();
	
}

void AHolisterAtk::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AHolisterAtk::Enable_Implementation()
{
	Super::Enable_Implementation();
}

void AHolisterAtk::Disable_Implementation()
{
	Super::Disable_Implementation();
	if (SkillOwner == nullptr) return;
	SkillOwner = nullptr;
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
}

void AHolisterAtk::Pushed_Implementation()
{
	Super::Pushed_Implementation();
}

void AHolisterAtk::Obtained_Implementation()
{
	Super::Obtained_Implementation();
}

void AHolisterAtk::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void AHolisterAtk::StartSkill()
{
	if(ACorePlayerCharacter* LocPlayerCharacter = Cast<ACorePlayerCharacter>(SkillOwner))
	{
		AttachToComponent(LocPlayerCharacter->SkillRootScene, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		TObjectPtr<UStaticMeshComponent> LocHolisterShotgunMesh;
		LocPlayerCharacter->GetWeaponStaticComponent(LocHolisterShotgunMesh);
		HolisterAtkFXComponent->AttachToComponent(LocHolisterShotgunMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("SKT_Shoot"));
	}
}

void AHolisterAtk::CancelSkill()
{
	SkillHitEnable(false);
	SkillFXEnable(false);
	Super::CancelSkill();
}

void AHolisterAtk::FinishSkill()
{
	Super::FinishSkill();
}

void AHolisterAtk::SkillHitEnable(bool InbEnable)
{
	if(InbEnable)
	{
		HolisterAtkCollisionComponent->SetCollisionEnabled(ECollisionEnabled::Type::QueryOnly);
		MakeSkillNoise();
		HolisterAtkCollisionComponent->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	}
	else
	{
		HolisterAtkCollisionComponent->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	}
}

void AHolisterAtk::SkillFXEnable(bool InbEnable, int32 InIndex)
{
	if(!HolisterAtkFXComponent->GetAsset()) return;
	if(InbEnable)
	{
		HolisterAtkFXComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		HolisterAtkFXComponent->Activate(true);
	}
	else
	{
		HolisterAtkFXComponent->Deactivate();
	}
}

void AHolisterAtk::BoxCollisionBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
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

