// Fill out your copyright notice in the Description page of Project Settings.


#include "NoxSkill.h"
#include "NiagaraComponent.h"
#include "Components/SphereComponent.h"
#include "ProjectT/System/Core/Characters/NPC/CoreNpc.h"
#include "ProjectT/System/Core/Characters/Player/CorePlayerCharacter.h"
#include "ProjectT/System/Core/Common/GlobalUtilsLibrary.h"
#include "ProjectT/System/Core/Managers/SoundManager.h"
#include "ProjectT/System/SinglePlay/Characters/NPC/SPNpcFixed.h"

ANoxSkill::ANoxSkill() :
	NoxSkillRootScene(nullptr),
	WandFXComponent(nullptr),
	SurgeFXComponent(nullptr),
	NoxSkillHitCollisionComponent(nullptr),
	SurgeSFXComponent(nullptr),
	SurgeWaveDelayMax(1.f / 3.f),
	SurgeWaveDelay(0.f),
	AppearLength(0.3f),
	ActivateTime(0.f),
	SurgeFXSwitchValue(0.f)
{
	SkillPoolsize = 2;
	SkillRowName = "312304";
	SkillId = 312304;
	InitializeSkillData();
	PrimaryActorTick.bCanEverTick = true;

	NoxSkillRootScene =CreateDefaultSubobject<USceneComponent>(TEXT("NoxSkillRootScene"));
	WandFXComponent=CreateDefaultSubobject<UNiagaraComponent>(TEXT("WandFXComponent"));
	SurgeFXComponent=CreateDefaultSubobject<UNiagaraComponent>(TEXT("ExplosionFXComponent"));
	NoxSkillHitCollisionComponent=CreateDefaultSubobject<USphereComponent>(TEXT("NoxSkillHitCollisionComponent"));

	NoxSkillRootScene->SetupAttachment(RootComponent);
	WandFXComponent->SetupAttachment(RootComponent);
	WandFXComponent->bAutoActivate = false;
	SurgeFXComponent->SetupAttachment(RootComponent);
	SurgeFXComponent->bAutoActivate = false;

	NoxSkillHitCollisionComponent->SetupAttachment(RootComponent);
	NoxSkillHitCollisionComponent->SetCollisionProfileName(TEXT("PlayerHit"));
	NoxSkillHitCollisionComponent->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	NoxSkillHitCollisionComponent->SetSphereRadius(8.f);
	NoxSkillHitCollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &ANoxSkill::SkillAreaBeginOverlap);
	NoxSkillHitCollisionComponent->OnComponentEndOverlap.AddDynamic(this, &ANoxSkill::SkillAreaEndOverlap);

	BodyHitComponent = NoxSkillHitCollisionComponent;
	RangeHitComponent = NoxSkillHitCollisionComponent;
}

void ANoxSkill::BeginPlay()
{
	Super::BeginPlay();
	
}

void ANoxSkill::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if(CurrentAffectNpc.Num() > 0)
	{
		for(ACoreNpc* LocSurgedNpc : CurrentAffectNpc)
		{
			FVector LocSubtract(GetActorLocation().X - LocSurgedNpc->GetActorLocation().X, GetActorLocation().Y - LocSurgedNpc->GetActorLocation().Y, 0.f);
			if(LocSubtract.Length() > 20)
			{
				if(UWorld* LocWorld = UGlobalUtilsLibrary::GetValidWorld(this))
				{
				LocSubtract.Normalize();
				FVector LocSurgedOffset = LocSubtract * (SkillEffects[0].fEffectValue * LocWorld->DeltaTimeSeconds);
				LocSurgedNpc->AddActorWorldOffset(LocSurgedOffset, true);
				}
			}
		}
	}	
}

void ANoxSkill::Enable_Implementation()
{
	Super::Enable_Implementation();
}

void ANoxSkill::Disable_Implementation()
{
	Super::Disable_Implementation();
	ResetDefaultState();
}

void ANoxSkill::Pushed_Implementation()
{
	Super::Pushed_Implementation();
}

void ANoxSkill::Obtained_Implementation()
{
	Super::Obtained_Implementation();
}

void ANoxSkill::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if(UWorld* LocWorld = UGlobalUtilsLibrary::GetValidWorld(this))
	{
		LocWorld->GetTimerManager().ClearTimer(SurgeFXDeactivate);	
	}
	Super::EndPlay(EndPlayReason);
}

void ANoxSkill::StartSkill()
{
	if(bAimState) ReleaseSkill();
}

void ANoxSkill::AimSkill()
{
	ACorePlayerCharacter* LocPlayerCharacter = Cast<ACorePlayerCharacter>(SkillOwner);
	if(!LocPlayerCharacter) return;
	bCanceled = false;
	bAimState = true;
	AttachToActor(LocPlayerCharacter,FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	TObjectPtr<UStaticMeshComponent> LocNoxWandMesh;
	LocPlayerCharacter->GetWeaponStaticComponent(LocNoxWandMesh);
	WandFXComponent->AttachToComponent(LocNoxWandMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("SKT_Gem"));
	SetActorTickEnabled(true);	
}

void ANoxSkill::ActivateSkill()
{
	if(bActivated) return;
	bAimState = false;
	bActivated = true;
}

void ANoxSkill::ReleaseSkill()
{
	bCanceled = true;
	bReleased = true;
	FinishSkill();
}

void ANoxSkill::CancelSkill()
{
	bCanceled = true;
	FinishSkill();
}

void ANoxSkill::FinishSkill()
{
	Super::FinishSkill();
}

void ANoxSkill::SkillHitEnable(bool InbEnable)
{
	if(InbEnable)
	{
		MakeSkillNoise(true);
		NoxSkillHitCollisionComponent->SetSphereRadius(SkillData.fEffectRange);
		NoxSkillHitCollisionComponent->SetCollisionEnabled(ECollisionEnabled::Type::QueryOnly);
		ACorePlayerCharacter* LocPlayerCharacter = Cast<ACorePlayerCharacter>(SkillOwner);
		LocPlayerCharacter->SetCurrentSkill(false);
		DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		SetActorLocation(CursorCalibrateHitResult.Location);
		SurgeFXComponent->Activate(true);
		SurgeFXComponent->SetFloatParameter(TEXT("BlackHole Switch"), SurgeFXSwitchValue);
		if(UWorld* LocWorld = UGlobalUtilsLibrary::GetValidWorld(this))
		{
			LocWorld->GetTimerManager().SetTimer(SurgeActivate, this, &ANoxSkill::CheckSurgeActivateTime, LocWorld->DeltaTimeSeconds, true);
		}
	}
	else
	{
		SurgeFXComponent->Deactivate();
		NoxSkillHitCollisionComponent->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
		if(UWorld* LocWorld = UGlobalUtilsLibrary::GetValidWorld(this))
		{
			LocWorld->GetTimerManager().SetTimer(SurgeFXDeactivate, this, &ANoxSkill::CheckSurgeDeactivate, LocWorld->DeltaTimeSeconds, true);
		}
	}
}

void ANoxSkill::SkillFXEnable(bool InbEnable, int32 InIndex)
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

void ANoxSkill::ResetDefaultState()
{
	Super::ResetDefaultState();
	SetActorTickEnabled(false);
	if (SkillOwner != nullptr)
	{
		SkillOwner = nullptr;
	}
	SurgeFXSwitchValue = 0.f;
	ActivateTime = 0.f;
	bCanceled = false;
	bAimState = false;
	bActivated = false;
	bReleased = false;
	SurgeSFXComponent = nullptr;
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	WandFXComponent->Deactivate();
	CurrentAffectNpc.Empty();
}

void ANoxSkill::CheckSurgeActivateTime()
{
	const UWorld* LocWorld = UGlobalUtilsLibrary::GetValidWorld(this);
	if(LocWorld == nullptr) return;
	SurgeWaveDelay + LocWorld->DeltaTimeSeconds < SurgeWaveDelayMax ? SurgeWaveDelay = SurgeWaveDelay + LocWorld->DeltaTimeSeconds : SurgeWaveDelay = SurgeWaveDelayMax;
	if(SurgeWaveDelay < SurgeWaveDelayMax) return;
	if(SurgeFXSwitchValue != 1.f)
	{
		SurgeFXSwitchValue + LocWorld->DeltaTimeSeconds / AppearLength > 1.f ? SurgeFXSwitchValue = 1.f : SurgeFXSwitchValue = SurgeFXSwitchValue + LocWorld->DeltaTimeSeconds / AppearLength;
		SurgeFXComponent->SetFloatParameter(TEXT("BlackHole Switch"), SurgeFXSwitchValue);
	}
	else
	{
		if(!SurgeSFXComponent)
			SurgeSFXComponent = UGlobalUtilsLibrary::GetSharedSoundManager(this)->PlaySound3DAttached(TEXT("831011"), RootComponent, EAttachLocation::Type::SnapToTarget,TEXT("None"));
	}
	ActivateTime = ActivateTime + LocWorld->DeltaTimeSeconds;
	if(ActivateTime > SkillEffects[0].fEffectDuration)
	{
		RecoverySurgedNpc();
		NoxSkillHitCollisionComponent->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
		SurgeWaveDelay = 0.f;
		LocWorld->GetTimerManager().ClearTimer(SurgeActivate);
		LocWorld->GetTimerManager().SetTimer(SurgeFXDeactivate, this, &ANoxSkill::CheckSurgeDeactivate, LocWorld->DeltaTimeSeconds, true);
	}
}

void ANoxSkill::CheckSurgeDeactivate()
{
	const UWorld* LocWorld = UGlobalUtilsLibrary::GetValidWorld(this);
	if(LocWorld == nullptr) return;
	if(SurgeFXComponent->IsActive())
	{
		if(SurgeFXSwitchValue - LocWorld->DeltaTimeSeconds / AppearLength > 0.f)
		{
			SurgeFXSwitchValue = SurgeFXSwitchValue - LocWorld->DeltaTimeSeconds / AppearLength;
			SurgeFXComponent->SetFloatParameter(TEXT("BlackHole Switch"), SurgeFXSwitchValue);
		}
		else
		{
			SurgeFXComponent->SetFloatParameter(TEXT("BlackHole Switch"), SurgeFXSwitchValue);
			SurgeFXComponent->Deactivate();
		}	
	}
	else
	{
		LocWorld->GetTimerManager().ClearTimer(SurgeFXDeactivate);
		SurgeFXDeactivate.Invalidate();
		FinishSkill();
	}
}

void ANoxSkill::RecoverySurgedNpc()
{
	if(CurrentAffectNpc.Num() > 0)
	{
		for(ACoreNpc* LocNpc : CurrentAffectNpc)
		{
			IDamageable::Execute_ApplyRecoveryEvent(LocNpc, SkillEffects);
		}
	}		
}

void ANoxSkill::SkillAreaBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(ACoreNpc* LocNpc = Cast<ACoreNpc>(OtherActor))
	{
		if(!Cast<ASPNpcFixed>(OtherActor))
		{
			IDamageable::Execute_ApplyDamageEvent(LocNpc, Cast<ACombatCharacter>(SkillOwner), GetActorLocation(), SkillData.fDamage, SkillEffects);
			CurrentAffectNpc.Add(LocNpc);
		}		
	}
}

void ANoxSkill::SkillAreaEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if(ACoreNpc* LocNpc = Cast<ACoreNpc>(OtherActor))
	{
		if(!Cast<ASPNpcFixed>(OtherActor))
		CurrentAffectNpc.Remove(LocNpc);
	}
}