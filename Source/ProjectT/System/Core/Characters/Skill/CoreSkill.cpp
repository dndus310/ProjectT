// Fill out your copyright notice in the Description page of Project Settings.


#include "CoreSkill.h"

#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "Components/SphereComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "ProjectT/Data/Gen/GenerateEnumDataTableKey.h"
#include "ProjectT/System/Core/Characters/NPC/CoreNpc.h"
#include "ProjectT/System/Core/Characters/Player/CorePlayerCharacter.h"
#include "ProjectT/System/Core/Common/AssetUtilsLibrary.h"
#include "ProjectT/System/Core/Common/GlobalUtilsLibrary.h"
#include "ProjectT/System/Core/GameModes/WorldGameMode.h"
#include "ProjectT/System/Core/Managers/CoreCheatManager.h"
#include "ProjectT/System/Core/Managers/PooledActorManager.h"

ACoreSkill::ACoreSkill():
	SkillRowName(TEXT("None")),
	RootSceneComponent(nullptr),
	NoiseSphereComponent(nullptr),
	HitFXSystem(nullptr),
	NoiseFXComponent(nullptr),
	SkillPoolsize(10),
	SkillOwner(nullptr),
	SkillId(0),
	bAimState(false),
	bCanceled(false),
	bReleased(false),
	bActivated(false),
	BodyHitComponent(nullptr),
	RangeHitComponent(nullptr)
{
	PrimaryActorTick.bCanEverTick = true;

	RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));
	NoiseSphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("NoiseSphereComponent"));
	NoiseFXComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NoiseFXComponent"));
	
	RootComponent = RootSceneComponent;

	NoiseSphereComponent->SetupAttachment(RootSceneComponent);
	NoiseSphereComponent->SetCollisionProfileName(TEXT("PlayerHit"));
	NoiseSphereComponent->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	NoiseFXComponent->SetupAttachment(NoiseSphereComponent);
	NoiseFXComponent->bAutoActivate = false;
}

void ACoreSkill::InitializeSkillData()
{
	if(SkillRowName.IsNone()) return;
	LinearTraceQuery = UEngineTypes::ConvertToTraceType(ECC_GameTraceChannel3);
	UDataTable* LocDataTable = UAssetUtilsLibrary::LoadDataTableByEnumKey(EDataTableKey::Skill);
	FSkill* LocSkillDataRow = LocDataTable->FindRow<FSkill>(SkillRowName, TEXT("Not Found FSkill"));
	if(!NMT_ENSURE(LocSkillDataRow)) return;
	SkillData = *LocSkillDataRow;
	TArray<int32> LocEffectArray;
	if(SkillData.iEffect1Id != 0) LocEffectArray.Add(SkillData.iEffect1Id);
	if(SkillData.iEffect2Id != 0) LocEffectArray.Add(SkillData.iEffect2Id);
	if(SkillData.iEffect3Id != 0) LocEffectArray.Add(SkillData.iEffect3Id);
	if(LocEffectArray.Num() > 0) 
	for(int32 LocEffectID : LocEffectArray)
	{
		FName LocEffectRowName = *FString::FromInt(LocEffectID);
		LocDataTable = UAssetUtilsLibrary::LoadDataTableByEnumKey(EDataTableKey::Effect);
		FEffect* LocEffectDataRow = LocDataTable->FindRow<FEffect>(LocEffectRowName, TEXT("Not Found FSkill"));
		SkillEffects.Add(*LocEffectDataRow);
	}
	NoiseSphereComponent->SetSphereRadius(SkillData.fSkillNoise);
	//TODO : Load Relative Path From Table After Added 
	if(UNiagaraSystem* LocFindNoiseAsset = Cast<UNiagaraSystem>(StaticLoadObject(UNiagaraSystem::StaticClass(),nullptr,TEXT("/Script/Niagara.NiagaraSystem'/Game/FX/NiagaraSystem/Common/NS_Common_SoundEangeEffects.NS_Common_SoundEangeEffects'"))))
		NoiseFXComponent->SetAsset(LocFindNoiseAsset);

	if(UNiagaraSystem* LocFindNoiseAsset = Cast<UNiagaraSystem>(StaticLoadObject(UNiagaraSystem::StaticClass(),nullptr,TEXT("/Script/Niagara.NiagaraSystem'/Game/FX/NiagaraSystem/Common/NS_Common_Hit.NS_Common_Hit'"))))
		HitFXSystem = LocFindNoiseAsset;

	
}

void ACoreSkill::SetSkillOwner(AActor* InSkillOwner)
{
	SkillOwner = InSkillOwner;
}

void ACoreSkill::GetSkillData(FSkill& OutCurrentSkillData)
{
	OutCurrentSkillData = SkillData;
}

float ACoreSkill::GetFinalDamage(AActor* InHitActor, bool InbLocationSelf)
{
	float LocFinalDamage = 0.f;
	if(SkillOwner)
	{
		ACorePlayerCharacter* LocPlayerCharacter = Cast<ACorePlayerCharacter>(SkillOwner);
		FVector2D LocOccurForward;
		InbLocationSelf	? LocOccurForward = FVector2D(InHitActor->GetActorLocation().X - GetActorLocation().X, InHitActor->GetActorLocation().Y - GetActorLocation().Y)
						: LocOccurForward = FVector2D(LocPlayerCharacter->GetMesh()->GetForwardVector().X, LocPlayerCharacter->GetMesh()->GetForwardVector().Y);
		FVector2D LocNpcForward(InHitActor->GetActorForwardVector().X,InHitActor->GetActorForwardVector().Y);
		float LocDotValue = UKismetMathLibrary::DotProduct2D(LocOccurForward,LocNpcForward);
		if(LocDotValue >= 0)
			SkillData.fCriDmg <= 0.f ? LocFinalDamage = SkillData.fDamage : LocFinalDamage = SkillData.fCriDmg;
		else
			LocFinalDamage = SkillData.fDamage;
	}
	return LocFinalDamage; 
}

void ACoreSkill::SetParabolaLaunchVelocity()
{
	FVector LocEndPoint = CursorCalibrateHitResult.Location;
	FVector LocCalibLocation(GetActorLocation().X, GetActorLocation().Y, LocEndPoint.Z);
	float LocFinalDistance = FVector::Distance(LocCalibLocation, LocEndPoint);
	UGlobalUtilsLibrary::SuggestProjectileVelocityAtLocation(GetWorld(), LaunchVelocity, GetActorLocation(),
															 LocEndPoint, 110.f, 0.f,
															 LocFinalDistance / 100.f);
}

UPrimitiveComponent* ACoreSkill::GetBodyHitComponent() const
{
	return BodyHitComponent;
}

UPrimitiveComponent* ACoreSkill::GetRangeHitComponent() const
{
	return RangeHitComponent;
}

void ACoreSkill::StartSkill()
{
}

void ACoreSkill::AimSkill()
{
}

void ACoreSkill::ActivateSkill()
{
}

void ACoreSkill::ReleaseSkill()
{
}

void ACoreSkill::CancelSkill()
{
}

void ACoreSkill::FinishSkill()
{
	UWorld* World = UGlobalUtilsLibrary::GetValidWorld(this);
	AWorldGameMode* LocGameMode = UGlobalUtilsLibrary::GetGameModeChecked<AWorldGameMode>(World);
	UPooledActorManager* LocPoolManager = IWorldObjectProvider::Execute_GetPooledActorManager(LocGameMode);
	if(LocPoolManager)
	{
		LocPoolManager->Push(this);
	}
}

void ACoreSkill::SkillHitEnable(bool InbEnable)
{
}

void ACoreSkill::SkillObjectHitEnable(bool InbEnable)
{
}

void ACoreSkill::SkillFXEnable(bool InbEnable, int32 InIndex)
{
}

void ACoreSkill::MakeSkillNoise(bool InbLocationSelf, UPrimitiveComponent* InAttachedComponent)
{
	if(SkillData.fSkillNoise == 0) return;
	if(InbLocationSelf)
	{
		if(InAttachedComponent != nullptr)
		{
			NoiseFXComponent->AttachToComponent(InAttachedComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		}
	}
	else
	{
		if(ACorePlayerCharacter* LocPlayerCharacter = Cast<ACorePlayerCharacter>(SkillOwner))
		{
			NoiseFXComponent->AttachToComponent(LocPlayerCharacter->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		}
	}	
	NoiseFXComponent->Activate(true);
	NoiseFXComponent->SetFloatParameter(TEXT("Wave Radius"), SkillData.fSkillNoise * 2.f);
	NoiseSphereComponent->SetSphereRadius(SkillData.fSkillNoise);
	NoiseSphereComponent->SetCollisionEnabled(ECollisionEnabled::Type::QueryOnly);
	TArray<AActor*> OverlappedActors;
	NoiseSphereComponent->GetOverlappingActors(OverlappedActors, ACoreNpc::StaticClass());
	TArray<AActor*> LocIgnoreActors;
	if(OverlappedActors.Num() >= 1)
	{
		for(AActor* LocOverlapActor : OverlappedActors)
		{
			if(ACoreNpc* LocOverlapNpc = Cast<ACoreNpc>(LocOverlapActor))
			{
				LocOverlapNpc->SetDetectLocationFromNoise(GetActorLocation());
			}
		}
	}
	NoiseSphereComponent->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
}

void ACoreSkill::ResetDefaultState()
{	
}

void ACoreSkill::MovedComponentToHitPoint(AActor* InHitActor, UPrimitiveComponent* InMovedComponent)
{
	TArray<AActor*> LocIgnoreActors;
	LocIgnoreActors.Add(SkillOwner);
	FHitResult LocTraceHitResult;
	UKismetSystemLibrary::LineTraceSingle(this, GetActorLocation(), InHitActor->GetActorLocation(),
										   TraceTypeQuery1, false, LocIgnoreActors, EDrawDebugTrace::None, LocTraceHitResult, true);
	if(LocTraceHitResult.bBlockingHit)
	{
		InMovedComponent->SetWorldLocation(LocTraceHitResult.Location);
	}
}

void ACoreSkill::BeginPlay()
{
	Super::BeginPlay();
}

void ACoreSkill::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACoreSkill::Enable_Implementation()
{
	IPoolableActor::Enable_Implementation();
}

void ACoreSkill::Disable_Implementation()
{
	IPoolableActor::Disable_Implementation();
	if(UCoreCheatManager* CCM = UGlobalUtilsLibrary::GetSharedCoreCheatManager(this))
	{
		if(CCM->GetDebugStatus().bDebugHitCollision)
		{
			if(BodyHitComponent) BodyHitComponent->SetHiddenInGame(true);
			if(RangeHitComponent) RangeHitComponent->SetHiddenInGame(true);
		}
	}	
}

void ACoreSkill::Pushed_Implementation()
{
	IPoolableActor::Pushed_Implementation();
}

void ACoreSkill::Obtained_Implementation()
{
	IPoolableActor::Obtained_Implementation();
	if(UCoreCheatManager* CCM = UGlobalUtilsLibrary::GetSharedCoreCheatManager(this))
	{
		if(CCM->GetDebugStatus().bDebugHitCollision)
		{
			BodyHitComponent->SetHiddenInGame(false);
			RangeHitComponent->SetHiddenInGame(false);
		}
	}	
}

void ACoreSkill::RefreshCursorHitResult_Implementation(FHitResult InHitResult)
{
	IMouseInteraction::RefreshCursorHitResult_Implementation(InHitResult);
	CursorCalibrateHitResult = InHitResult;	
}
