// Fill out your copyright notice in the Description page of Project Settings.


#include "ParabolaVisualObject.h"

#include "Kismet/KismetMathLibrary.h"
#include "ProjectT/System/Core/Characters/Player/CorePlayerCharacter.h"
#include "ProjectT/System/Core/Common/GlobalUtilsLibrary.h"
#include "ProjectT/System/Core/GameModes/WorldGameMode.h"

AParabolaVisualObject::AParabolaVisualObject()
{
	PrimaryActorTick.bCanEverTick = true;
	AimSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("AimSceneComponent"));
	ParabolaMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ParabolaMeshComponent"));
	AffectAreaMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AffectAreaMeshComponent"));

	ParabolaMeshComponent->SetupAttachment(AimSceneComponent);
	ParabolaMeshComponent->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	ParabolaMeshComponent->SetHiddenInGame(true);
	AffectAreaMeshComponent->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	AffectAreaMeshComponent->SetHiddenInGame(true);
}

void AParabolaVisualObject::BeginPlay()
{
	Super::BeginPlay();
	SetActorTickEnabled(false);
}

void AParabolaVisualObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AParabolaVisualObject::RefreshCursorHitResult_Implementation(FHitResult InHitResult)
{
	IMouseInteraction::RefreshCursorHitResult_Implementation(InHitResult);
	CursorHitResult = InHitResult;
	FRotator LocCursorRotator(FRotator(0.f, UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), CursorHitResult.Location).Yaw, 0.f));
	AimSceneComponent->SetWorldRotation(LocCursorRotator);
	FVector LocEndPoint = CursorHitResult.Location;
	AffectAreaMeshComponent->SetWorldLocation(LocEndPoint + FVector(0, 0, 0.1));
	FVector LocCalibLocation(GetActorLocation().X, GetActorLocation().Y, LocEndPoint.Z);
	float LocFinalDistance = FVector::Distance(LocCalibLocation, LocEndPoint);
	ParabolaMeshComponent->SetWorldScale3D(FVector(LocFinalDistance / 100.f, 1.f, 1.1f));
}

void AParabolaVisualObject::InitializeParabolaObject()
{
	if(AWorldGameMode* LocGameMode = UGlobalUtilsLibrary::GetGameModeChecked<AWorldGameMode>(this))
	{
		if(ACorePlayerCharacter* LocPlayerCharacter = LocGameMode->GetPlayerCharacter())
		{
			AttachToActor(LocPlayerCharacter, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		}
	}	
}

void AParabolaVisualObject::SetAimVisibility(bool InbVisible, bool InbPathVisible, float InAffectRange)
{
	if(InbVisible) AffectAreaMeshComponent->SetWorldScale3D(FVector(InAffectRange / 50.f));
	AffectAreaMeshComponent->SetHiddenInGame(!InbVisible);
	if(InbPathVisible) ParabolaMeshComponent->SetHiddenInGame(!InbVisible);
}

