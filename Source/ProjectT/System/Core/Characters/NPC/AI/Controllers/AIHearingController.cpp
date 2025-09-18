// Fill out your copyright notice in the Description page of Project Settings.


#include "AIHearingController.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "ProjectT/ProjectT.h"
#include "ProjectT/System/Core/Interfaces/AISettings.h"


AAIHearingController::AAIHearingController(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	HearingConfig = AddHearingConfig();
}

void AAIHearingController::BeginPlay()
{
	Super::BeginPlay();
}

void AAIHearingController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	NMT_CHECKF(HearingConfig);
	NMT_CHECKF(GetPawn());

	if(GetPawn()->GetClass()->ImplementsInterface(UAISettings::StaticClass()))
	{
		FHearingInfo HearingInfo = IAISettings::Execute_GetHearingInfo(InPawn);

		HearingConfig->HearingRange = HearingInfo.HearingRange;
		HearingConfig->SetMaxAge(HearingInfo.HearingMaxAge);
	}

	UpdatePerceptionSystem();
}

void AAIHearingController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
