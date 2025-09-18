// Fill out your copyright notice in the Description page of Project Settings.


#include "IcienAtk.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/SphereComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "ProjectT/System/Core/Characters/NPC/CoreNpc.h"
#include "ProjectT/System/Core/Characters/Player/CorePlayerCharacter.h"
#include "ProjectT/System/Core/Common/GlobalUtilsLibrary.h"
#include "ProjectT/System/Core/Managers/SoundManager.h"

AIcienAtk::AIcienAtk() :
	IcienAtkRootScene(nullptr),
	IcienAtkHitCollisionComponent(nullptr),
	IcienAtkFXComponent(nullptr),
	IcienHandLFXComponent(nullptr),
	IcienHandRFXComponent(nullptr),
	AtkSFXComponent(nullptr)
{
	SkillPoolsize = 5;
	SkillRowName = "311305";
	SkillId = 311305;
	InitializeSkillData();
	PrimaryActorTick.bCanEverTick = true;

	IcienAtkRootScene = CreateDefaultSubobject<USceneComponent>(TEXT("IcienAtkRootScene"));
	IcienAtkHitCollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("IcienAtkHitCollisionComponent"));
	IcienAtkFXComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("IcienAtkFXComponent"));
	IcienHandLFXComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("IcienHandLFXComponent"));
	IcienHandRFXComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("IcienHandRFXComponent"));

	IcienAtkRootScene->SetupAttachment(RootComponent);
	IcienAtkHitCollisionComponent->SetupAttachment(RootComponent);
	IcienAtkHitCollisionComponent->SetSphereRadius(SkillData.fEffectRange);
	IcienAtkHitCollisionComponent->SetCollisionProfileName(TEXT("PlayerHit"));
	IcienAtkHitCollisionComponent->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	
	IcienAtkFXComponent->SetupAttachment(IcienAtkHitCollisionComponent);
	IcienAtkFXComponent->bAutoActivate = false;
	IcienHandLFXComponent->SetupAttachment(RootComponent);
	IcienHandLFXComponent->bAutoActivate = false;
	IcienHandRFXComponent->SetupAttachment(RootComponent);
	IcienHandRFXComponent->bAutoActivate = false;

	NoiseSphereComponent->SetupAttachment(IcienAtkHitCollisionComponent);
}

void AIcienAtk::BeginPlay()
{
	Super::BeginPlay();
}

void AIcienAtk::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AIcienAtk::Enable_Implementation()
{
	Super::Enable_Implementation();
}

void AIcienAtk::Disable_Implementation()
{
	Super::Disable_Implementation();
	ResetDefaultState();
}

void AIcienAtk::Pushed_Implementation()
{
	Super::Pushed_Implementation();
}

void AIcienAtk::Obtained_Implementation()
{
	Super::Obtained_Implementation();
}

void AIcienAtk::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if(UWorld* LocWorld = UGlobalUtilsLibrary::GetValidWorld(this))
	{
		LocWorld->GetTimerManager().ClearTimer(FXDeactivate);	
	}
	Super::EndPlay(EndPlayReason);
}

void AIcienAtk::StartSkill()
{
	if(ACorePlayerCharacter* LocPlayerCharacter = Cast<ACorePlayerCharacter>(SkillOwner))
	{
		AttachToActor(LocPlayerCharacter,FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		IcienHandLFXComponent->AttachToComponent(LocPlayerCharacter->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("SKT_L_Hand"));
		IcienHandRFXComponent->AttachToComponent(LocPlayerCharacter->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("SKT_R_Hand"));
		IcienAtkHitCollisionComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		IcienAtkHitCollisionComponent->SetWorldLocation(CursorCalibrateHitResult.Location);
	}
}

void AIcienAtk::ActivateSkill()
{
	AtkSFXComponent = UGlobalUtilsLibrary::GetSharedSoundManager(this)->PlaySound3DAttached(TEXT("831012"), IcienAtkHitCollisionComponent, EAttachLocation::Type::SnapToTarget,TEXT("None"));
	bActivated = true;
	IcienAtkFXComponent->Activate(true);
	IcienAtkHitCollisionComponent->SetCollisionEnabled(ECollisionEnabled::Type::QueryOnly);
	TArray<AActor*> OverlappedActors;
	IcienAtkHitCollisionComponent->GetOverlappingActors(OverlappedActors, ACoreNpc::StaticClass());
	if (OverlappedActors.Num() >= 1)
	{
		for (AActor* LocOverlapActor : OverlappedActors)
		{
			TArray<AActor*> LocIgnoreActors;
			FHitResult LocTraceHitResult;
			FVector LocTraceStart(IcienAtkHitCollisionComponent->GetComponentLocation().X, IcienAtkHitCollisionComponent->GetComponentLocation().Y, GetActorLocation().Z);
			UKismetSystemLibrary::LineTraceSingle(this, LocTraceStart, LocOverlapActor->GetActorLocation(),
			                                      LinearTraceQuery, false, LocIgnoreActors, EDrawDebugTrace::None, LocTraceHitResult, true);
			if(!LocTraceHitResult.bBlockingHit)
			{
				IDamageable::Execute_ApplyDamageEvent(LocOverlapActor, Cast<ACombatCharacter>(SkillOwner), GetActorLocation(), GetFinalDamage(LocOverlapActor), SkillEffects);
				if(UWorld* LocWorld = UGlobalUtilsLibrary::GetValidWorld(this)) UNiagaraFunctionLibrary::SpawnSystemAtLocation(LocWorld, HitFXSystem, LocOverlapActor->GetActorLocation());
			}			
		}
	}
	IcienAtkHitCollisionComponent->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	if(UWorld* LocWorld = UGlobalUtilsLibrary::GetValidWorld(this))
	{
		LocWorld->GetTimerManager().SetTimer(FXDeactivate, this, &AIcienAtk::CheckFXDeactivate, LocWorld->DeltaTimeSeconds, true);	
	}	
	MakeSkillNoise(true, IcienAtkHitCollisionComponent);
}

void AIcienAtk::CancelSkill()
{
	if(!bActivated)
	{
		bCanceled = true;
		SkillHitEnable(false);
		SkillFXEnable(false);
		Super::CancelSkill();
	}	
}

void AIcienAtk::FinishSkill()
{
	Super::FinishSkill();
}

void AIcienAtk::SkillHitEnable(bool InbEnable)
{
	if(InbEnable)
	{
		ActivateSkill();
	}
}

void AIcienAtk::SkillFXEnable(bool InbEnable, int32 InIndex)
{
	if(InbEnable)
	{
		IcienHandLFXComponent->Activate(true);
		IcienHandRFXComponent->Activate(true);
	}
	else
	{
		IcienHandLFXComponent->Deactivate();
		IcienHandRFXComponent->Deactivate();
	}
}

void AIcienAtk::ResetDefaultState()
{
	Super::ResetDefaultState();
	if (SkillOwner != nullptr)
	{
		SkillOwner = nullptr;
	}
	bCanceled = false;
	bActivated = false;
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
}

void AIcienAtk::CheckFXDeactivate()
{
	if(IcienAtkFXComponent->IsActive()) return;
	if(UWorld* LocWorld = UGlobalUtilsLibrary::GetValidWorld(this))
	{
		LocWorld->GetTimerManager().ClearTimer(FXDeactivate);
		FinishSkill();
	}
}

