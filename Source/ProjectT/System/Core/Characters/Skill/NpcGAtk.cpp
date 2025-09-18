// Fill out your copyright notice in the Description page of Project Settings.


#include "NpcGAtk.h"

#include "NiagaraComponent.h"
#include "Components/BoxComponent.h"
#include "ProjectT/System/Core/Characters/Player/CorePlayerCharacter.h"
#include "ProjectT/System/SinglePlay/Characters/NPC/SPNpcAgent.h"

ANpcGAtk::ANpcGAtk() :
	MeleeAttackCollisionComponent(nullptr),
	MeleeSwingFXComponent(nullptr),
	MeleeHitFXComponent(nullptr),
	SkillNpcOwner(nullptr)
{
	SkillRowName = "311152";
	SkillId = 311152;
	SkillPoolsize = 5;
	InitializeSkillData();
	PrimaryActorTick.bCanEverTick = true;

	MeleeAttackCollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("MeleeAttackCollisionComponent"));
	MeleeSwingFXComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("MeleeSwingFXComponent"));
	MeleeHitFXComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("MeleeHitFXComponent"));
	NMT_CHECKF(MeleeAttackCollisionComponent && MeleeSwingFXComponent && MeleeHitFXComponent)
	
	MeleeAttackCollisionComponent->SetCollisionProfileName(TEXT("EnemyHit"));
	MeleeAttackCollisionComponent->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	MeleeAttackCollisionComponent->SetupAttachment(RootComponent);
	MeleeAttackCollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &ANpcGAtk::DefaultCollisionBeginOverlap);

	MeleeSwingFXComponent->SetupAttachment(MeleeAttackCollisionComponent);
	MeleeSwingFXComponent->bAutoActivate = false;

	MeleeHitFXComponent->SetupAttachment(MeleeAttackCollisionComponent);
	MeleeHitFXComponent->bAutoActivate = false;

	BodyHitComponent = MeleeAttackCollisionComponent;
	RangeHitComponent = MeleeAttackCollisionComponent;
}

void ANpcGAtk::BeginPlay()
{
	Super::BeginPlay();

	SetActorTickEnabled(false);
}

void ANpcGAtk::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ANpcGAtk::Enable_Implementation()
{
	Super::Enable_Implementation();
}

void ANpcGAtk::Disable_Implementation()
{
	Super::Disable_Implementation();

	ResetDefaultState();
}

void ANpcGAtk::Pushed_Implementation()
{
	Super::Pushed_Implementation();
}

void ANpcGAtk::Obtained_Implementation()
{
	Super::Obtained_Implementation();
}

void ANpcGAtk::SkillFXEnable(bool InbEnable, int32 InIndex)
{
	if(InbEnable)
	{
		MeleeSwingFXComponent->Activate(true);
	}
	else
	{
		MeleeSwingFXComponent->Deactivate();
	}
}

void ANpcGAtk:: ResetDefaultState()
{
	Super::ResetDefaultState();

	SetActorTickEnabled(false);
	SetSkillOwner(nullptr);
	SkillHitEnable(false);
	SkillFXEnable(false);
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	SetActorLocation(FVector::ZeroVector);
}

void ANpcGAtk::StartSkill()
{
	SkillNpcOwner = Cast<ASPNpcAgent>(SkillOwner);
	if(SkillNpcOwner)
	{
		AttachToActor(SkillNpcOwner, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		MeleeSwingFXComponent->AttachToComponent(SkillNpcOwner->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("SKT_AgentG_Baton_R"));
		SetActorTickEnabled(true);
	}
}

void ANpcGAtk::FinishSkill()
{
	Super::FinishSkill();
}

void ANpcGAtk::SkillHitEnable(bool InbEnable)
{
	if(!SkillNpcOwner)
		return;

	if(InbEnable)
	{
		MeleeAttackCollisionComponent->SetCollisionEnabled(ECollisionEnabled::Type::QueryOnly);
	}
	else
	{	
		MeleeAttackCollisionComponent->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	}
}

void ANpcGAtk::DefaultCollisionBeginOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                            const FHitResult& SweepResult)
{
	if(Cast<ACorePlayerCharacter>(OtherActor))
	{
		IDamageable::Execute_ApplyDamageEvent(OtherActor, Cast<ACombatCharacter>(SkillOwner), SkillNpcOwner->GetActorLocation(), SkillData.fDamage, SkillEffects);
		MeleeHitFXComponent->Activate(true);
		SkillHitEnable(false);
	}
}
