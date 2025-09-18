// Fill out your copyright notice in the Description page of Project Settings.


#include "SPLevelObject.h"


// Sets default values
ASPLevelObject::ASPLevelObject()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ASPLevelObject::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASPLevelObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

