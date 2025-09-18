// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_SelectRandomPatrolDir.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "ProjectT/System/Core/Characters/NPC/AI/Blackboard/BlackboardKeyLibrary.h"
#include "ProjectT/System/SinglePlay/Characters/NPC/SPNpcAgent.h"

UBTTask_SelectRandomPatrolDir::UBTTask_SelectRandomPatrolDir()
{
	NodeName = "Random Patrol Dir";
}

EBTNodeResult::Type UBTTask_SelectRandomPatrolDir::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ASPNpcAgent* AgentNpc = Cast<ASPNpcAgent>(OwnerComp.GetAIOwner()->GetPawn());
	if(!AgentNpc)
		return EBTNodeResult::Failed;
	
	UBlackboardComponent* BBC = OwnerComp.GetBlackboardComponent();
	if(!BBC)
		return EBTNodeResult::Failed;
	
	bool RandomDir = FMath::RandBool();
	BBC->SetValueAsBool(UBlackboardKeyLibrary::GetMoveReverseKey(), RandomDir);
	UE_LOG(LogTemp, Warning, TEXT("RandomDir %d"), RandomDir);

	return EBTNodeResult::Succeeded;
}
