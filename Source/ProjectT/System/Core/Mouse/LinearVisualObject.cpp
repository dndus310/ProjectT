// Fill out your copyright notice in the Description page of Project Settings.


#include "LinearVisualObject.h"
#include "Kismet/KismetMathLibrary.h"
#include "ProjectT/System/Core/Characters/Player/CorePlayerCharacter.h"
#include "ProjectT/System/Core/Common/GlobalUtilsLibrary.h"
#include "ProjectT/System/Core/GameModes/WorldGameMode.h"

ALinearVisualObject::ALinearVisualObject()
{
	PrimaryActorTick.bCanEverTick = true;
	AimSceneRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("AimSceneRootComponent"));
	AimSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("AimSceneComponent"));
	AimMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AimMeshComponent"));
	AimTipSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("AimTipSceneComponent"));
	AimTipMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AimTipMeshComponent"));

	AimSceneComponent->SetupAttachment(AimSceneRootComponent);
	AimSceneComponent->SetRelativeLocation(FVector(0.f, 0.f, -49.9f));

	AimMeshComponent->SetupAttachment(AimSceneComponent);
	AimMeshComponent->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	AimMeshComponent->CastShadow = false;
	AimMeshComponent->SetHiddenInGame(true);

	AimTipSceneComponent->SetupAttachment(AimSceneComponent);
	AimTipSceneComponent->SetRelativeLocation(FVector(100.f, 0.f, 0.f));
	AimTipMeshComponent->SetupAttachment(AimTipSceneComponent);
	AimTipMeshComponent->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	AimTipMeshComponent->CastShadow = false;
	AimTipMeshComponent->SetHiddenInGame(true);
	AimTipMeshComponent->SetUsingAbsoluteScale(true);
}

void ALinearVisualObject::BeginPlay()
{
	Super::BeginPlay();
	SetActorTickEnabled(false);	
}

void ALinearVisualObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ALinearVisualObject::RefreshCursorHitResult_Implementation(FHitResult InHitResult)
{
	IMouseInteraction::RefreshCursorHitResult_Implementation(InHitResult);
	CursorHitResult = InHitResult;
	FRotator LocCursorRotator(FRotator(0.f, UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), CursorHitResult.Location).Yaw, 0.f));
	AimSceneComponent->SetWorldRotation(LocCursorRotator);
	float LocRatio = (CursorHitResult.Distance / 100.f) - 0.1f;
	AimSceneComponent->SetRelativeScale3D(FVector(LocRatio, 1.f, 1.f));
	AimTipMeshComponent->SetRelativeLocation(FVector(-20.f / LocRatio, 0.f, 0.f));
}

void ALinearVisualObject::InitializeLinearObject()
{
	if(AWorldGameMode* LocGameMode = UGlobalUtilsLibrary::GetGameModeChecked<AWorldGameMode>(this))
	{
		if(ACorePlayerCharacter* LocPlayerCharacter = LocGameMode->GetPlayerCharacter())
		{
			AttachToActor(LocPlayerCharacter, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		}
	}	
}

void ALinearVisualObject::SetAimVisibility(const bool InbVisible)
{
	AimMeshComponent->SetHiddenInGame(!InbVisible);
	AimTipMeshComponent->SetHiddenInGame(!InbVisible);
}

