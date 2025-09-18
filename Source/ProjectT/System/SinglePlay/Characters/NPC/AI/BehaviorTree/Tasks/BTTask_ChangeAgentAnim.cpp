// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_ChangeAgentAnim.h"

#include "AIController.h"
#include "ProjectT/System/SinglePlay/Characters/NPC/SPNpcAgent.h"
#include "ProjectT/System/SinglePlay/Characters/NPC/Animations/SPNpcAgentAnimInstance.h"

UBTTask_ChangeAgentAnim::UBTTask_ChangeAgentAnim() :
	bNormal(true),
	NpcNormalAnimState(ENpcNormalAnimState::Idle),
	NpcAbnormalAnimState(ENpcAbnormalAnimState::HitBack)
{
	NodeName = TEXT("Change AgentAnim");
	bIgnoreRestartSelf = true;
}

EBTNodeResult::Type UBTTask_ChangeAgentAnim::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ASPNpcAgent* AgentNpc = Cast<ASPNpcAgent>(OwnerComp.GetAIOwner()->GetPawn());
	if(!AgentNpc)
		return EBTNodeResult::Failed;

	USPNpcAgentAnimInstance* AnimInst = AgentNpc->GetAgentAnimInstance();
	if(!AnimInst)
		return EBTNodeResult::Failed;

	if(bNormal)
	{
		AnimInst->SetNormalState(true);
		AnimInst->ChangeNpcNormalAnimState(NpcNormalAnimState);
	}
	else
	{
		AnimInst->ChangeNpcAbnormalAnimState(NpcAbnormalAnimState);
	}

	return EBTNodeResult::Succeeded;
}
