// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_CheckAtkDistance.h"

#include "AIController.h"
#include "RHIShaderFormatDefinitions.inl"
#include "BehaviorTree/BlackboardComponent.h"
#include "ProjectT/System/Core/Characters/NPC/AI/Blackboard/BlackboardKeyLibrary.h"
#include "ProjectT/System/SinglePlay/Characters/NPC/SPNpcAgent.h"

UBTService_CheckAtkDistance::UBTService_CheckAtkDistance()
{
	NodeName = TEXT("CheckAtkDistance");
	Interval = 1.f;
}

void UBTService_CheckAtkDistance::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	UBlackboardComponent* BBC = OwnerComp.GetAIOwner()->GetBlackboardComponent();
	if(!BBC)
		return;

	AActor* Target = Cast<AActor>(BBC->GetValueAsObject(UBlackboardKeyLibrary::GetTargetActorKey()));
	if(!Target)
		return;

	ASPNpcAgent* AgentNpc = Cast<ASPNpcAgent>(OwnerComp.GetAIOwner()->GetPawn());
	if(!AgentNpc)
		return;

	if(AgentNpc->IsTargetInAttackRange(Target) && AgentNpc->IsFinishSkillCoolDown() && BBC->GetValueAsBool(UBlackboardKeyLibrary::GetCanAttack()))
	{
		AgentNpc->ChangeNpcState(ENpcState::Attack);
	}
}
