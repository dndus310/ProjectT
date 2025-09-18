// Fill out your copyright notice in the Description page of Project Settings.


#include "SneakoAtk.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/BoxComponent.h"
#include "ProjectT/System/Core/Characters/NPC/CoreNpc.h"
#include "ProjectT/System/Core/Characters/Player/CorePlayerCharacter.h"
#include "ProjectT/System/Core/Common/GlobalUtilsLibrary.h"

ASneakoAtk::ASneakoAtk() :
	SneakoAtkRootSceneComponent(nullptr),
	SneakoAtkCollisionComponent(nullptr),
	SneakoAtkFXComponent(nullptr)
{
	SkillPoolsize = 3;
	SkillRowName = "311102";
	SkillId = 311102;
	InitializeSkillData();
	PrimaryActorTick.bCanEverTick = true;

	SneakoAtkRootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SneakoAtkRootSceneComponent"));
	SneakoAtkCollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("SneakoAtkCollisionComponent"));
	SneakoAtkFXComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("SneakoAtkFXComponent"));

	SneakoAtkRootSceneComponent->SetupAttachment(RootComponent);
	SneakoAtkCollisionComponent->SetupAttachment(SneakoAtkRootSceneComponent);
	SneakoAtkCollisionComponent->SetCollisionProfileName(TEXT("PlayerHit"));
	SneakoAtkCollisionComponent->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	SneakoAtkCollisionComponent->OnComponentBeginOverlap.AddDynamic(this,&ASneakoAtk::BoxCollisionBeginOverlap);

	BodyHitComponent = SneakoAtkCollisionComponent;
	RangeHitComponent = SneakoAtkCollisionComponent;

	SneakoAtkFXComponent->bAutoActivate = false;
}

void ASneakoAtk::BeginPlay()
{
	Super::BeginPlay();	
}

void ASneakoAtk::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASneakoAtk::Enable_Implementation()
{
	Super::Enable_Implementation();
}

void ASneakoAtk::Disable_Implementation()
{
	Super::Disable_Implementation();
	if(SkillOwner == nullptr) return;
	SneakoAtkFXComponent->Deactivate();
	SkillOwner = nullptr;
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
}

void ASneakoAtk::Pushed_Implementation()
{
	Super::Pushed_Implementation();
}

void ASneakoAtk::Obtained_Implementation()
{
	Super::Obtained_Implementation();
}

void ASneakoAtk::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if(UWorld* LocWorld = UGlobalUtilsLibrary::GetValidWorld(this))
	{
		LocWorld->GetTimerManager().ClearTimer(FXDeactivateTimerHandle);
	}
	Super::EndPlay(EndPlayReason);
}

void ASneakoAtk::StartSkill()
{
	if(ACorePlayerCharacter* LocPlayerCharacter = Cast<ACorePlayerCharacter>(SkillOwner))
	{
		AttachToComponent(LocPlayerCharacter->SkillRootScene, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		SneakoAtkFXComponent->AttachToComponent(LocPlayerCharacter->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("SKT_Sneako_Dagger_R"));
	}
}

void ASneakoAtk::CancelSkill()
{
	SkillHitEnable(false);
	SkillFXEnable(false);
	Super::FinishSkill();
}

void ASneakoAtk::FinishSkill()
{
	if(UWorld* LocWorld = UGlobalUtilsLibrary::GetValidWorld(this))
	{
		LocWorld->GetTimerManager().SetTimer(FXDeactivateTimerHandle, this, &ASneakoAtk::CheckFXDeactivated, LocWorld->DeltaTimeSeconds, true);
	}	
}

void ASneakoAtk::SkillHitEnable(bool InbEnable)
{
	if(InbEnable)
	{
		SneakoAtkCollisionComponent->SetCollisionEnabled(ECollisionEnabled::Type::QueryOnly);
		MakeSkillNoise();
		SneakoAtkCollisionComponent->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	}
	else
	{
		SneakoAtkCollisionComponent->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	}
}

void ASneakoAtk::SkillFXEnable(bool InbEnable, int32 InIndex)
{
	if(!SneakoAtkFXComponent->GetAsset()) return;
	if(InbEnable) SneakoAtkFXComponent->Activate(true);
}

void ASneakoAtk::CheckFXDeactivated()
{
	if(SneakoAtkFXComponent->IsActive()) return;
	if(UWorld* LocWorld = UGlobalUtilsLibrary::GetValidWorld(this))
	{
		LocWorld->GetTimerManager().ClearTimer(FXDeactivateTimerHandle);
		FinishSkill();
	}	
}

void ASneakoAtk::BoxCollisionBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(ACoreNpc* LocHitNpc = Cast<ACoreNpc>(OtherActor))
	{
		IDamageable::Execute_ApplyDamageEvent(LocHitNpc, Cast<ACombatCharacter>(SkillOwner), GetActorLocation(), GetFinalDamage(OtherActor), SkillEffects);
		if(UWorld* LocWorld = UGlobalUtilsLibrary::GetValidWorld(this)) UNiagaraFunctionLibrary::SpawnSystemAtLocation(LocWorld, HitFXSystem, LocHitNpc->GetActorLocation());
	}
}

