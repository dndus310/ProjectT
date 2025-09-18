// Fill out your copyright notice in the Description page of Project Settings.


#include "IcienSkill.h"

#include "NiagaraComponent.h"
#include "ProjectT/System/Core/Characters/Player/CorePlayerCharacter.h"
#include "ProjectT/System/Core/Common/GlobalUtilsLibrary.h"

AIcienSkill::AIcienSkill() :
	IcienSkillRootSceneComponent(nullptr),
	IcienSkillFXComponent(nullptr),
	IcienHandLFXComponent(nullptr),
	IcienHandRFXComponent(nullptr),
	CurrentActivateTime(0.f)
{
	SkillPoolsize = 2;
	SkillRowName = "312405";
	SkillId = 312405;
	InitializeSkillData();
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bCanEverTick = true;

	IcienSkillRootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("IcienSkillRootSceneComponent"));
	IcienSkillFXComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("IcienSkillFXComponent"));
	IcienHandLFXComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("IcienHandLFXComponent"));
	IcienHandRFXComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("IcienHandRFXComponent"));
	
	IcienSkillFXComponent->SetupAttachment(RootComponent);
	IcienSkillFXComponent->bAutoActivate = false;
	IcienHandLFXComponent->SetupAttachment(RootComponent);
	IcienHandLFXComponent->bAutoActivate = false;
	IcienHandRFXComponent->SetupAttachment(RootComponent);
	IcienHandRFXComponent->bAutoActivate = false;
}

void AIcienSkill::BeginPlay()
{
	Super::BeginPlay();
	
}

void AIcienSkill::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AIcienSkill::Enable_Implementation()
{
	Super::Enable_Implementation();
}

void AIcienSkill::Disable_Implementation()
{
	Super::Disable_Implementation();
	if (SkillOwner == nullptr) return;
	bActivated = false;
	bCanceled = false;
	SkillOwner = nullptr;
	if(UWorld* LocWorld = UGlobalUtilsLibrary::GetValidWorld(this)) LocWorld->GetTimerManager().ClearTimer(ActivateTimerHandle);
	CurrentActivateTime = 0.f;
	SkillTimeEnd.Unbind();
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
}

void AIcienSkill::Pushed_Implementation()
{
	Super::Pushed_Implementation();
}

void AIcienSkill::Obtained_Implementation()
{
	Super::Obtained_Implementation();
}

void AIcienSkill::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if(UWorld* LocWorld = UGlobalUtilsLibrary::GetValidWorld(this))
	{
		LocWorld->GetTimerManager().ClearTimer(ActivateTimerHandle);	
	}
	Super::EndPlay(EndPlayReason);
}

void AIcienSkill::StartSkill()
{
	if(ACorePlayerCharacter* LocPlayerCharacter = Cast<ACorePlayerCharacter>(SkillOwner))
	{
		AttachToActor(LocPlayerCharacter,FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		IcienSkillFXComponent->AttachToComponent(LocPlayerCharacter->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("SOK_Root_Fx"));
		IcienHandLFXComponent->AttachToComponent(LocPlayerCharacter->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("SKT_L_Hand"));
		IcienHandRFXComponent->AttachToComponent(LocPlayerCharacter->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("SKT_R_Hand"));
	}
}

void AIcienSkill::ActivateSkill()
{
	IcienSkillFXComponent->Activate(true);
	MakeSkillNoise();
	bActivated = true;
	IDamageable::Execute_ApplyDamageEvent(SkillOwner, nullptr, FVector(), 0.f, SkillEffects);
	if(UWorld* LocWorld = UGlobalUtilsLibrary::GetValidWorld(this))
	{
		LocWorld->GetTimerManager().SetTimer(ActivateTimerHandle, this, &AIcienSkill::CheckActiveTime, LocWorld->DeltaTimeSeconds, true);	
	}
}

void AIcienSkill::CancelSkill()
{
	if(!bActivated)
	{
		bCanceled = true;
		SkillHitEnable(false);
		SkillFXEnable(false);
		Super::CancelSkill();	
	}	
}

void AIcienSkill::FinishSkill()
{
	Super::FinishSkill();
}

void AIcienSkill::SkillHitEnable(bool InbEnable)
{
}

void AIcienSkill::SkillFXEnable(bool InbEnable, int32 InIndex)
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

void AIcienSkill::CheckActiveTime()
{
	if(UWorld* LocWorld = UGlobalUtilsLibrary::GetValidWorld(this))
	{
		CurrentActivateTime = CurrentActivateTime + LocWorld->DeltaTimeSeconds;
		if(CurrentActivateTime >= SkillEffects[0].fEffectDuration)
		{
			IcienSkillFXComponent->Deactivate();
			LocWorld->GetTimerManager().ClearTimer(ActivateTimerHandle);
			if(SkillTimeEnd.IsBound()) SkillTimeEnd.Execute();
		}
	}
}
