// Fill out your copyright notice in the Description page of Project Settings.


#include "AIPerceptionController.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Navigation/CrowdFollowingComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISense_Sight.h"
#include "ProjectT/System/Core/Characters/NPC/AI/Blackboard/BlackboardKeyLibrary.h"
#include "ProjectT/System/Core/Characters/Player/CorePlayerCharacter.h"
#include "ProjectT/System/Core/Interfaces/AISettings.h"
#include "ProjectT/System/SinglePlay/Characters/NPC/SPNpcAgent.h"


AAIPerceptionController::AAIPerceptionController(const FObjectInitializer& ObjectInitializer) : 
	Super(ObjectInitializer.SetDefaultSubobjectClass<UCrowdFollowingComponent>(TEXT("PathFollowingComponent")))
{
	SetPerceptionComponent(*CreateOptionalDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComponent")));
}

void AAIPerceptionController::BeginPlay()
{
	Super::BeginPlay();

	if(ASPNpcAgent* NpcAgent = Cast<ASPNpcAgent>(GetPawn()))
	{
		CrowdFollowingComponent = Cast<UCrowdFollowingComponent>(GetPathFollowingComponent());
		NMT_CHECKF(CrowdFollowingComponent);
		SetupCrowdSettings(NpcAgent->CrowdSettingsInfo);
	}
}

void AAIPerceptionController::SetupCrowdSettings(const FCrowdSettingsInfo& InCrowdSettings) const
{
	if(!NMT_ENSURE(CrowdFollowingComponent))
		return;

	if(InCrowdSettings.bUseCrowdSeparation)
	{
		CrowdFollowingComponent->SetCrowdSeparation(InCrowdSettings.bUseCrowdSeparation);
		CrowdFollowingComponent->SetCrowdSeparationWeight(InCrowdSettings.SeparationWeight);
		CrowdFollowingComponent->SetCrowdAvoidanceRangeMultiplier(InCrowdSettings.AvoidanceRangeMultiplier);
		CrowdFollowingComponent->SetCrowdRotateToVelocity(false);
	}
}

void AAIPerceptionController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
}

void AAIPerceptionController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AAIPerceptionController::StartAI()
{
	Super::StartAI();
	SetPerceptionActive(true);
}

void AAIPerceptionController::StopAI(const FString& InReason, const bool InbControlPerception)
{
	if(InbControlPerception)
		SetPerceptionActive(false);
	
	Super::StopAI(InReason, InbControlPerception);
}

void AAIPerceptionController::ActivateBehaviorTree(APawn* InPawn)
{
	Super::ActivateBehaviorTree(InPawn);
	
	if(InPawn->GetClass()->ImplementsInterface(UAISettings::StaticClass()))
	{
		bool bUseSense = IAISettings::Execute_GetUseSight(InPawn) || IAISettings::Execute_GetUseHearing(InPawn);
		if(bUseSense)
			SetPerceptionActive(true);
	}
}

void AAIPerceptionController::UpdatedTargetPerception(AActor* InTarget, FAIStimulus InStimulus)
{
	if(!NMT_ENSURE(GetPawn())) return;

	ACorePlayerCharacter* Player = Cast<ACorePlayerCharacter>(InTarget);
	if(GetPawn()->GetClass()->ImplementsInterface(UAISettings::StaticClass()) && Player)
	{
		if(InStimulus.WasSuccessfullySensed())
		{
			bool PlayerUndetect;
			Player->GetUnDetectState(PlayerUndetect);
			if(PlayerUndetect || bUnDetect)
				return;
			
			TSubclassOf<UAISense> Sense = UAIPerceptionSystem::GetSenseClassForStimulus(GetWorld(), InStimulus);

			if(IAISettings::Execute_GetUseSight(GetPawn()) && Sense == UAISense_Sight::StaticClass())
			{
				IAISettings::Execute_DetectTargetBySight(GetPawn(), InTarget, InStimulus);
			}
			
			UpdateTargetInfo(InTarget);
		}
		else
		{
			if(GetCheckedBlackboardComponent()->GetValueAsObject(UBlackboardKeyLibrary::GetTargetActorKey()) != nullptr)
			{
				IAISettings::Execute_ForgetTarget(GetPawn(), InTarget);
			}
		}
	}
}

void AAIPerceptionController::SetPerceptionActive(const bool InbEnable)
{
	NMT_CHECKF(PerceptionComponent)
	if(InbEnable)
	{
		PerceptionComponent->OnTargetPerceptionUpdated.AddUniqueDynamic(this, &AAIPerceptionController::UpdatedTargetPerception);
	}
	else
	{
		GetCheckedBlackboardComponent()->SetValueAsObject(UBlackboardKeyLibrary::GetTargetActorKey(), nullptr);
		PerceptionComponent->OnTargetPerceptionUpdated.Clear();
	}
}

void AAIPerceptionController::UpdatePerceptionSystem() const
{
	NMT_CHECKF(PerceptionComponent)
	PerceptionComponent->RequestStimuliListenerUpdate();
	PerceptionComponent->SetCanEverAffectNavigation(true);
}

void AAIPerceptionController::ForceUpdatePerception() const
{
	NMT_CHECKF(PerceptionComponent)
	if(PerceptionComponent->OnTargetPerceptionUpdated.IsBound())
	{
		TArray<AActor*> LocPerceivedActors;
		PerceptionComponent->GetCurrentlyPerceivedActors(UAISense_Sight::StaticClass(), LocPerceivedActors);
		if(LocPerceivedActors.Num() > 0)
		{
			for(AActor* LocPerceiveActor : LocPerceivedActors)
			{
				FActorPerceptionBlueprintInfo LocPerceptionInfo;
				PerceptionComponent->GetActorsPerception(LocPerceiveActor, LocPerceptionInfo);
				PerceptionComponent->OnTargetPerceptionUpdated.Broadcast(LocPerceiveActor, LocPerceptionInfo.LastSensedStimuli[0]);
			}
		}
	}
}

class UAISenseConfig_Sight* AAIPerceptionController::AddSightConfig()
{
	UAISenseConfig_Sight* SightConfig = CreateOptionalDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = false;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = false;
	
	NMT_CHECKF(PerceptionComponent)
	PerceptionComponent->SetDominantSense(*SightConfig->GetSenseImplementation());
	PerceptionComponent->ConfigureSense(*SightConfig);

	return SightConfig;
}

class UAISenseConfig_Hearing* AAIPerceptionController::AddHearingConfig()
{
	UAISenseConfig_Hearing* HearingConfig = CreateOptionalDefaultSubobject<UAISenseConfig_Hearing>(TEXT("HearingConfig"));
	HearingConfig->DetectionByAffiliation.bDetectEnemies = true;
	HearingConfig->DetectionByAffiliation.bDetectNeutrals = false;
	HearingConfig->DetectionByAffiliation.bDetectFriendlies = false;

	NMT_CHECKF(PerceptionComponent)
	PerceptionComponent->SetDominantSense(*HearingConfig->GetSenseImplementation());
	PerceptionComponent->ConfigureSense(*HearingConfig);

	return HearingConfig;
}
