// Fill out your copyright notice in the Description page of Project Settings.


#include "AISightHearingController.h"

#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AISenseConfig_Sight.h"
#include "ProjectT/ProjectT.h"
#include "ProjectT/System/Core/Interfaces/AISettings.h"

AAISightHearingController::AAISightHearingController(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	HearingConfig = AddHearingConfig();
	SightConfig = AddSightConfig();
}

void AAISightHearingController::BeginPlay()
{
	Super::BeginPlay();
}

void AAISightHearingController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	NMT_CHECKF(SightConfig && HearingConfig);
	NMT_CHECKF(GetPawn());

	if(GetPawn()->GetClass()->ImplementsInterface(UAISettings::StaticClass()))
	{
		FSightInfo SightInfo = IAISettings::Execute_GetSightInfo(GetPawn());

		SightConfig->SightRadius = SightInfo.SightRange;
		SightConfig->LoseSightRadius = SightInfo.LoseSightRange;
		SightConfig->PeripheralVisionAngleDegrees = SightInfo.SightAngle / 2.f;
		SightConfig->SetMaxAge(SightInfo.SightMaxAge);

		FHearingInfo HearingInfo = IAISettings::Execute_GetHearingInfo(InPawn);

		HearingConfig->HearingRange = HearingInfo.HearingRange;
		HearingConfig->SetMaxAge(HearingInfo.HearingMaxAge);
	}

	UpdatePerceptionSystem();
}

void AAISightHearingController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
