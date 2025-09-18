// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_ChangeSpeed.h"

#include "AIController.h"
#include "ProjectT/System/SinglePlay/Characters/NPC/SPNpcAgent.h"

UBTTask_ChangeSpeed::UBTTask_ChangeSpeed() :
	SpeedType(ENpcSpeedType::Walk)
{
	NodeName = TEXT("ChangeSpeed");
	bIgnoreRestartSelf = true;
}

EBTNodeResult::Type UBTTask_ChangeSpeed::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ASPNpcAgent* AgentNpc = Cast<ASPNpcAgent>(OwnerComp.GetAIOwner()->GetPawn());
	if(!AgentNpc)
		return EBTNodeResult::Failed;

	AgentNpc->ChangeSpeed(SpeedType);

	return EBTNodeResult::Succeeded;
}
