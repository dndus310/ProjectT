// Fill out your copyright notice in the Description page of Project Settings.


#include "SneakoSkill.h"

#include "NiagaraComponent.h"
#include "Components/AudioComponent.h"
#include "ProjectT/System/Core/Characters/Player/CorePlayerCharacter.h"
#include "ProjectT/System/Core/Common/GlobalUtilsLibrary.h"
#include "ProjectT/System/Core/Managers/SoundManager.h"

ASneakoSkill::ASneakoSkill() :
	SneakoSkillRootSceneComponent(nullptr),
	SneakoSkillMeshComponent(nullptr),
	SneakoSkillFXComponent(nullptr),
	TransformSFXComponent(nullptr),
	CurrentActivateTime(0.f)
{
	SkillPoolsize = 2;
	SkillRowName = "312402";
	SkillId = 312402;
	InitializeSkillData();
	PrimaryActorTick.bCanEverTick = true;
	
	SneakoSkillRootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SneakoSkillRootSceneComponent"));
	SneakoSkillMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SneakoSkillMeshComponent"));
	SneakoSkillFXComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("SneakoSkillFXComponent"));
	
	SneakoSkillRootSceneComponent->SetupAttachment(RootComponent);
	SneakoSkillMeshComponent->SetupAttachment(RootComponent);
	SneakoSkillMeshComponent->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	SneakoSkillMeshComponent->SetHiddenInGame(true);
	SneakoSkillFXComponent->bAutoActivate = false;
}

void ASneakoSkill::BeginPlay()
{
	Super::BeginPlay();
}

void ASneakoSkill::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASneakoSkill::Enable_Implementation()
{
	Super::Enable_Implementation();
}

void ASneakoSkill::Disable_Implementation()
{
	Super::Disable_Implementation();
	if (SkillOwner == nullptr) return;
	bActivated = false;
	if(UWorld* LocWorld = UGlobalUtilsLibrary::GetValidWorld(this)) LocWorld->GetTimerManager().ClearTimer(ActivateTimerHandle);
	CurrentActivateTime = 0.f;
	SneakoSkillMeshComponent->SetHiddenInGame(true);
	SkillTimeEnd.Unbind();
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
}

void ASneakoSkill::Pushed_Implementation()
{
	Super::Pushed_Implementation();
}

void ASneakoSkill::Obtained_Implementation()
{
	Super::Obtained_Implementation();
}

void ASneakoSkill::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if(UWorld* LocWorld = UGlobalUtilsLibrary::GetValidWorld(this))
	{
		LocWorld->GetTimerManager().ClearTimer(ActivateTimerHandle);	
	}
	Super::EndPlay(EndPlayReason);
}

void ASneakoSkill::StartSkill()
{
	bActivated ? ReleaseSkill() : ActivateSkill(); 
}

void ASneakoSkill::ActivateSkill()
{
	if(ACorePlayerCharacter* LocPlayerCharacter = Cast<ACorePlayerCharacter>(SkillOwner))
	{
		AttachToActor(LocPlayerCharacter, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		MakeSkillNoise();
		if(TransformSFXComponent) TransformSFXComponent.Get()->Stop();
		TransformSFXComponent = UGlobalUtilsLibrary::GetSharedSoundManager(this)->PlaySound3DAttached(TEXT("831005"), RootComponent, EAttachLocation::Type::SnapToTarget,TEXT("None"));
		bActivated = true;
		IDamageable::Execute_ApplyDamageEvent(SkillOwner, nullptr, FVector(), 0.f, SkillEffects);
		if(UWorld* LocWorld = UGlobalUtilsLibrary::GetValidWorld(this))
		{
			LocWorld->GetTimerManager().SetTimer(ActivateTimerHandle, this, &ASneakoSkill::CheckActiveTime, LocWorld->DeltaTimeSeconds, true);
		}
		SneakoSkillMeshComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		SneakoSkillMeshComponent->SetRelativeRotation(FRotator(0.f, 0.f, 0.f));
		SneakoSkillMeshComponent->SetHiddenInGame(false);
		SneakoSkillFXComponent->AttachToComponent(LocPlayerCharacter->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("SKT_Sneako_Box"));
		SneakoSkillMeshComponent->AttachToComponent(LocPlayerCharacter->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("SKT_Sneako_Box"));
		SneakoSkillFXComponent->Activate(true);			
	}
}

void ASneakoSkill::ReleaseSkill()
{
	IDamageable::Execute_ApplyRecoveryEvent(SkillOwner, SkillEffects);
	Super::ReleaseSkill();
	FinishSkill();
}

void ASneakoSkill::CancelSkill()
{
	IDamageable::Execute_ApplyRecoveryEvent(SkillOwner, SkillEffects);
	Super::CancelSkill();
	FinishSkill();
}

void ASneakoSkill::FinishSkill()
{
	if(TransformSFXComponent) TransformSFXComponent.Get()->Stop();
	TransformSFXComponent = UGlobalUtilsLibrary::GetSharedSoundManager(this)->PlaySound3DAttached(TEXT("831005"), RootComponent, EAttachLocation::Type::SnapToTarget,TEXT("None"));
	SneakoSkillFXComponent->Activate(true);
	Super::FinishSkill();
}

void ASneakoSkill::SkillHitEnable(bool InbEnable)
{
}

void ASneakoSkill::ResetDefaultState()
{
	Super::ResetDefaultState();
}

void ASneakoSkill::CheckActiveTime()
{
	const UWorld* LocWorld = UGlobalUtilsLibrary::GetValidWorld(this);
	if(LocWorld == nullptr) return;
	CurrentActivateTime = CurrentActivateTime + LocWorld->DeltaTimeSeconds;
	if(CurrentActivateTime >= SkillEffects[0].fEffectDuration)
	{
		LocWorld->GetTimerManager().ClearTimer(ActivateTimerHandle);
		if(SkillTimeEnd.IsBound()) SkillTimeEnd.Execute();
	}
}

