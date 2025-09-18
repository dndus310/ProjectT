// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_MoveToLocation.h"

#include "AIController.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "ProjectT/System/Core/Characters/NPC/CoreNpc.h"
#include "ProjectT/System/Core/Characters/NPC/AI/Blackboard/BlackboardKeyLibrary.h"
#include "ProjectT/System/SinglePlay/Characters/NPC/SPNpcAgent.h"

UBTTask_MoveToLocation::UBTTask_MoveToLocation() :
	AgentNpc(nullptr)
{
	NodeName = TEXT("Move To Location");
}

EBTNodeResult::Type UBTTask_MoveToLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AgentNpc = OwnerComp.GetAIOwner()->GetPawn<ASPNpcAgent>();
	if(!AgentNpc)
		return EBTNodeResult::Failed;

	AAIController* AIController = OwnerComp.GetAIOwner();
	AIController->ClearFocus(EAIFocusPriority::Default);
	AgentNpc->SetHeadRotationEnable(false);

	return PerformMoveTask(OwnerComp, NodeMemory);
}

void UBTTask_MoveToLocation::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);

	if(TaskResult == EBTNodeResult::Succeeded)
	{
		ArriveToTargetLocation(OwnerComp);
	}
	else if(TaskResult == EBTNodeResult::Failed)
	{
		NMT_LOG("Move Fail")
		ArriveToTargetLocation(OwnerComp);
	}
	else if(TaskResult == EBTNodeResult::Aborted)
	{
		NMT_LOG("Move Aborted")
		ArriveToTargetLocation(OwnerComp);
	}
}

void UBTTask_MoveToLocation::ArriveToTargetLocation(UBehaviorTreeComponent& OwnerComp) const
{
	UBlackboardComponent* BBC = OwnerComp.GetBlackboardComponent();
	if(!BBC)
		return;

	if(!AgentNpc)
		return;
	
	if(BBC->GetValueAsObject(UBlackboardKeyLibrary::GetTargetActorKey()) == nullptr)
	{
		if(AgentNpc->GetNpcState() == ENpcState::Chase)
		{
			AgentNpc->ChangeNpcState(ENpcState::Guard);
			NMT_LOG("Arrive Target Location. To Guard");
		}
	}
}
