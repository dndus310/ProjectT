// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_SelectNextPatrolType.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "ProjectT/System/SinglePlay/Characters/NPC/SPNpcPatrol.h"

UBTTask_SelectNextPatrolType::UBTTask_SelectNextPatrolType()
{
	NodeName = "Select NextPatrolType";
}

EBTNodeResult::Type UBTTask_SelectNextPatrolType::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ASPNpcPatrol* PatrolNpc = Cast<ASPNpcPatrol>(OwnerComp.GetAIOwner()->GetPawn());
	if(!PatrolNpc)
		return EBTNodeResult::Failed;

	PatrolNpc->ChangePatrolType();

	return EBTNodeResult::Succeeded;
}
