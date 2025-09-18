// Fill out your copyright notice in the Description page of Project Settings.


#include "BTDecorator_CheckHp.h"

#include "AIController.h"
#include "ProjectT/System/Core/Characters/NPC/CoreNpc.h"
#include "ProjectT/System/SinglePlay/Components/SPNpcStatusComponent.h"

UBTDecorator_CheckHp::UBTDecorator_CheckHp()
{
	NodeName = "CheckHp";
}

bool UBTDecorator_CheckHp::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	ACoreNpc* Npc = Cast<ACoreNpc>(OwnerComp.GetAIOwner()->GetPawn());
	if(!Npc)
		return false;

	USPNpcStatusComponent* NpcStatusComponent = Npc->GetNpcStatusComponent();
	if(!NpcStatusComponent)
		return false;

	return NpcStatusComponent->GetCurrentHP() > 0.f;
}
