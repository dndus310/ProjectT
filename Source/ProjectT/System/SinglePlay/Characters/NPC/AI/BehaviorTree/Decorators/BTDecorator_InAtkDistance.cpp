// Fill out your copyright notice in the Description page of Project Settings.


#include "BTDecorator_InAtkDistance.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "ProjectT/System/Core/Characters/NPC/AI/Blackboard/BlackboardKeyLibrary.h"
#include "ProjectT/System/SinglePlay/Characters/NPC/SPNpcAgent.h"

UBTDecorator_InAtkDistance::UBTDecorator_InAtkDistance()
{
	NodeName = "In Atk Distance";
}

bool UBTDecorator_InAtkDistance::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	UBlackboardComponent* BBC = OwnerComp.GetAIOwner()->GetBlackboardComponent();
	if(!BBC)
		return false;

	AActor* Target = Cast<AActor>(BBC->GetValueAsObject(UBlackboardKeyLibrary::GetTargetActorKey()));
	if(!Target)
		return false;

	ASPNpcAgent* AgentNpc = Cast<ASPNpcAgent>(OwnerComp.GetAIOwner()->GetPawn());
	if(!AgentNpc)
		return false;

	return AgentNpc->IsTargetInAttackRange(Target);
}
