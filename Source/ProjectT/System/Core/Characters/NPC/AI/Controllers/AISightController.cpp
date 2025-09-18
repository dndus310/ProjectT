// Fill out your copyright notice in the Description page of Project Settings.


#include "AISightController.h"

#include "Perception/AISenseConfig_Sight.h"
#include "ProjectT/ProjectT.h"
#include "ProjectT/System/Core/Interfaces/AISettings.h"


AAISightController::AAISightController(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	SightConfig = AddSightConfig();
}

void AAISightController::BeginPlay()
{
	Super::BeginPlay();
}

void AAISightController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	NMT_CHECKF(SightConfig);
	NMT_CHECKF(GetPawn());

	if(GetPawn()->GetClass()->ImplementsInterface(UAISettings::StaticClass()))
	{
		FSightInfo SightInfo = IAISettings::Execute_GetSightInfo(GetPawn());

		SightConfig->SightRadius = SightInfo.SightRange;
		SightConfig->LoseSightRadius = SightInfo.LoseSightRange;
		SightConfig->PeripheralVisionAngleDegrees = SightInfo.SightAngle / 2.f;
		SightConfig->SetMaxAge(SightInfo.SightMaxAge);
	}

	UpdatePerceptionSystem();
}

void AAISightController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
