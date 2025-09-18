// Fill out your copyright notice in the Description page of Project Settings.


#include "CoreObject.h"


ACoreObject::ACoreObject()
{
	PrimaryActorTick.bCanEverTick = true;

	MeshSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("MeshSceneComponent"));
	MeshSceneComponent->SetupAttachment(RootComponent);
}

void ACoreObject::ShowVisual_Implementation()
{
	IVisualizable::ShowVisual_Implementation();
}

void ACoreObject::HideVisual_Implementation()
{
	IVisualizable::HideVisual_Implementation();
}

void ACoreObject::BeginPlay()
{
	Super::BeginPlay();
}

void ACoreObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

