// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_Attack.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "ProjectT/System/Core/Characters/NPC/AI/Controllers/AIControllerBase.h"
#include "ProjectT/System/SinglePlay/Characters/NPC/SPNpcAgent.h"

UBTTask_Attack::UBTTask_Attack()
{
	NodeName = TEXT("Attack");
}

EBTNodeResult::Type UBTTask_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ASPNpcAgent* AgentNpc = Cast<ASPNpcAgent>(OwnerComp.GetAIOwner()->GetPawn());
	if(!AgentNpc)
		return EBTNodeResult::Failed;

	AAIControllerBase* AIC = Cast<AAIControllerBase>(OwnerComp.GetAIOwner());
	if(!AIC)
		return EBTNodeResult::Failed;
	
	AgentNpc->TryAttack();
	
	return EBTNodeResult::Succeeded;
}

