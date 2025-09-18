// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_Patrol.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "ProjectT/ProjectT.h"
#include "ProjectT/System/Core/Characters/NPC/AI/Blackboard/BlackboardKeyLibrary.h"
#include "ProjectT/System/Core/Components/PathFollowComponent.h"

UBTTask_Patrol::UBTTask_Patrol()
{
	NodeName = TEXT("Patrol");
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_Patrol::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	APawn* OwnerPawn = OwnerComp.GetAIOwner()->GetPawn();
	if(!NMT_ENSURE(OwnerPawn))
		return EBTNodeResult::Failed;

	UPathFollowComponent* PathFollowComponent = OwnerPawn->FindComponentByClass<UPathFollowComponent>();
	if(!NMT_ENSURE(PathFollowComponent))
		return EBTNodeResult::Failed;

	UBlackboardComponent* BBC = OwnerComp.GetBlackboardComponent();
	if(!BBC)
		return EBTNodeResult::Failed;

	if(bool bMoveReverse = BBC->GetValueAsBool(UBlackboardKeyLibrary::GetMoveReverseKey()))
	{
		PathFollowComponent->MoveToNextWayPoint(bMoveReverse);
		BBC->SetValueAsBool(UBlackboardKeyLibrary::GetMoveReverseKey(), false);
	}
	else
		PathFollowComponent->MoveToNextWayPoint();

	return EBTNodeResult::InProgress;
}

void UBTTask_Patrol::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
}

void UBTTask_Patrol::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);
}
