// Fill out your copyright notice in the Description page of Project Settings.


#include "AIControllerBase.h"

#include "BrainComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BlackboardData.h"
#include "ProjectT/ProjectT.h"
#include "ProjectT/System/Core/Interfaces/AISettings.h"
#include "ProjectT/Data/Npc/NpcDataModel.h"
#include "ProjectT/System/Core/Characters/NPC/AI/Blackboard/BlackboardKeyLibrary.h"

AAIControllerBase::AAIControllerBase(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer),
	bCanDetect(true)
{
}

void AAIControllerBase::BeginPlay()
{
	Super::BeginPlay();
}

class UBlackboardComponent* AAIControllerBase::GetCheckedBlackboardComponent()
{
	UBlackboardComponent* BBC = GetBlackboardComponent();
	NMT_CHECKF(BBC)
	return BBC;
}

void AAIControllerBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void AAIControllerBase::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	SetGenericTeamId(FGenericTeamId(0));
}

void AAIControllerBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AAIControllerBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	OnDetectTarget.Clear();
	OnForgetTarget.Clear();
}

void AAIControllerBase::UpdateTargetInfo(AActor* InTarget)
{
	GetCheckedBlackboardComponent()->SetValueAsObject(UBlackboardKeyLibrary::GetTargetActorKey(), InTarget);

	if(InTarget != nullptr)
	{
		UpdateTargetLocation(InTarget->GetActorLocation());
		UpdateTargetDirection(InTarget);

		OnDetectTarget.Broadcast();
	}
	else
	{
		OnForgetTarget.Broadcast();
	}
}

void AAIControllerBase::UpdateTargetLocation(const FVector& InLocation)
{
	GetCheckedBlackboardComponent()->SetValueAsVector(UBlackboardKeyLibrary::GetTargetLocationKey(), InLocation);
}

void AAIControllerBase::UpdateTargetDirection(const AActor* InTarget)
{
	FVector Dir = InTarget->GetActorLocation() - GetPawn()->GetActorLocation();
	Dir.Normalize();
	GetCheckedBlackboardComponent()->SetValueAsVector(UBlackboardKeyLibrary::GetTargetDirKey(), Dir);
}

void AAIControllerBase::UpdateTargetDirection(const FVector& InTargetDirection)
{
	GetCheckedBlackboardComponent()->SetValueAsVector(UBlackboardKeyLibrary::GetTargetDirKey(), InTargetDirection);
}

void AAIControllerBase::ChangeNpcStateKey(const ENpcState InState)
{
	GetCheckedBlackboardComponent()->SetValueAsEnum(UBlackboardKeyLibrary::GetCurrentStateKey(), static_cast<uint8>(InState));
}

void AAIControllerBase::StartAI()
{
	NMT_CHECKF(BrainComponent)
	BrainComponent->StartLogic();
}

void AAIControllerBase::StopAI(const FString& Reason, const bool bControlPerception)
{
	NMT_CHECKF(BrainComponent)
	BrainComponent->StopLogic(Reason);
}

AActor* AAIControllerBase::GetTargetActor() const
{
	return Cast<AActor>(GetBlackboardComponent()->GetValueAsObject(UBlackboardKeyLibrary::GetTargetActorKey()));
}

void AAIControllerBase::ActivateBehaviorTree(APawn* InPawn)
{
	if(InPawn->GetClass()->ImplementsInterface(UAISettings::StaticClass()))
	{
		UBlackboardData* BB = IAISettings::Execute_GetBlackboardData(InPawn);
		BehaviorTree = IAISettings::Execute_GetBehaviorTree(InPawn);

		if(!NMT_ENSURE(BB && BehaviorTree)) return;

		UBlackboardComponent* BlackboardComponent = Blackboard.Get();

		if(UseBlackboard(BB, BlackboardComponent))
		{
			RunBehaviorTree(BehaviorTree);
		}
	}
}

void AAIControllerBase::UpdateReturnLocation(const FVector& InLocation)
{
	GetCheckedBlackboardComponent()->SetValueAsVector(UBlackboardKeyLibrary::GetReturnLocationKey(), InLocation);
}

void AAIControllerBase::UpdateNextPatrolType(const EPatrolType InPatrolType)
{
	GetCheckedBlackboardComponent()->SetValueAsEnum(UBlackboardKeyLibrary::GetPatrolTypeKey(), static_cast<uint8>(InPatrolType));
}

void AAIControllerBase::UpdateDoubtState(const EDoubtState InDoubtState)
{
	GetCheckedBlackboardComponent()->SetValueAsEnum(UBlackboardKeyLibrary::GetCurrentDoubtState(), static_cast<uint8>(InDoubtState));
}

void AAIControllerBase::SetEnableAttack(const bool InbEnable)
{
	GetCheckedBlackboardComponent()->SetValueAsBool(UBlackboardKeyLibrary::GetCanAttack(), InbEnable);
}

void AAIControllerBase::SetCanRotateWhileAttack(const bool InbEnable)
{
	GetCheckedBlackboardComponent()->SetValueAsBool(UBlackboardKeyLibrary::GetCanRotateWhileAttack(), InbEnable);
}
