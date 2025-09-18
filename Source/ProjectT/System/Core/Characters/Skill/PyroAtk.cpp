// Fill out your copyright notice in the Description page of Project Settings.


#include "PyroAtk.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/BoxComponent.h"
#include "ProjectT/System/Core/Characters/Player/CorePlayerCharacter.h"
#include "ProjectT/System/Core/Characters/NPC/CoreNpc.h"
#include "ProjectT/System/Core/Common/GlobalUtilsLibrary.h"

APyroAtk::APyroAtk() :
	PyroAtkRootSceneComponent(nullptr),
	PyroAtkCollisionComponent(nullptr),
	PyroAtkFXComponent(nullptr)
{
	SkillPoolsize = 2;
	SkillRowName = "311101";
	SkillId = 311101;
	InitializeSkillData();
	PrimaryActorTick.bCanEverTick = true;

	PyroAtkRootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("PyroAtkRootSceneComponent"));
	PyroAtkCollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("PyroAtkCollisionComponent"));
	PyroAtkFXComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("PyroAtkFXComponent"));
	
	PyroAtkRootSceneComponent->SetupAttachment(RootComponent);
	PyroAtkCollisionComponent->SetupAttachment(PyroAtkRootSceneComponent);
	PyroAtkCollisionComponent->SetCollisionProfileName(TEXT("PlayerHit"));
	PyroAtkCollisionComponent->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	PyroAtkCollisionComponent->OnComponentBeginOverlap.AddDynamic(this,&APyroAtk::BoxCollisionBeginOverlap);
	
	PyroAtkFXComponent->SetupAttachment(RootComponent);
	PyroAtkFXComponent->bAutoActivate = false;
	
	BodyHitComponent = PyroAtkCollisionComponent;
	RangeHitComponent = PyroAtkCollisionComponent;
}

void APyroAtk::BeginPlay()
{
	Super::BeginPlay();
}

void APyroAtk::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APyroAtk::Enable_Implementation()
{
	Super::Enable_Implementation();
}

void APyroAtk::Disable_Implementation()
{
	Super::Disable_Implementation();
	if (SkillOwner == nullptr) return;
	SkillOwner = nullptr;
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
}

void APyroAtk::Pushed_Implementation()
{
	Super::Pushed_Implementation();
}

void APyroAtk::Obtained_Implementation()
{
	Super::Obtained_Implementation();
}

void APyroAtk::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void APyroAtk::StartSkill()
{
	;
	if(ACorePlayerCharacter* LocPlayerCharacter = Cast<ACorePlayerCharacter>(SkillOwner))
	{
		AttachToComponent(LocPlayerCharacter->SkillRootScene,FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		PyroAtkFXComponent->AttachToComponent(LocPlayerCharacter->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("SKT_Pyro_Firepunch_L"));
	}
}

void APyroAtk::CancelSkill()
{
	SkillHitEnable(false);
	SkillFXEnable(false);
	Super::FinishSkill();
}

void APyroAtk::FinishSkill()
{
	Super::FinishSkill();
}

void APyroAtk::SkillHitEnable(bool InbEnable)
{
	if(InbEnable)
	{
		PyroAtkCollisionComponent->SetCollisionEnabled(ECollisionEnabled::Type::QueryOnly);
		MakeSkillNoise();
	}
	else
	{
		PyroAtkCollisionComponent->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	}
}

void APyroAtk::SkillFXEnable(bool InbEnable, int32 InIndex)
{
	if(InbEnable)
	{
		PyroAtkFXComponent->Activate(true);
	}
	else
	{
		PyroAtkFXComponent->Deactivate();
	}
}

void APyroAtk::BoxCollisionBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(ACoreNpc* LocHitNpc = Cast<ACoreNpc>(OtherActor))
	{
		IDamageable::Execute_ApplyDamageEvent(LocHitNpc, Cast<ACombatCharacter>(SkillOwner), GetActorLocation(), GetFinalDamage(LocHitNpc), SkillEffects);
		if(UWorld* LocWorld = UGlobalUtilsLibrary::GetValidWorld(this)) UNiagaraFunctionLibrary::SpawnSystemAtLocation(LocWorld, HitFXSystem, LocHitNpc->GetActorLocation());
	}
}
