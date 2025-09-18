// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_Alert.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "ProjectT/System/Core/Characters/NPC/CoreNpc.h"
#include "ProjectT/System/Core/Characters/NPC/AI/Blackboard/BlackboardKeyLibrary.h"

UBTTask_Alert::UBTTask_Alert()
{
	NodeName = TEXT("Alert");
	bIgnoreRestartSelf = true;
}

EBTNodeResult::Type UBTTask_Alert::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ACoreNpc* Npc = Cast<ACoreNpc>(OwnerComp.GetAIOwner()->GetPawn());
	if(!Npc)
		return EBTNodeResult::Failed;

	UBlackboardComponent* BBC = OwnerComp.GetBlackboardComponent();
	if(!BBC)
		return EBTNodeResult::Failed;

	FVector TargetLocation = BBC->GetValueAsVector(UBlackboardKeyLibrary::GetTargetLocationKey());
	Npc->Alert(TargetLocation);

	return EBTNodeResult::Succeeded;
}
