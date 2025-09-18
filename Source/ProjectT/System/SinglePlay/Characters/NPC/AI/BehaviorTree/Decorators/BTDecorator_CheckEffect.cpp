// Fill out your copyright notice in the Description page of Project Settings.


#include "BTDecorator_CheckEffect.h"

#include "AIController.h"
#include "ProjectT/Data/Gen/GenerateEnumEffectSubTypes.h"
#include "ProjectT/System/Core/Characters/NPC/CoreNpc.h"

UBTDecorator_CheckEffect::UBTDecorator_CheckEffect() :
	EffectSubType(EEffectSubTypes::None)
{
	NodeName = "CheckEffect";
}

bool UBTDecorator_CheckEffect::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	ACoreNpc* Npc = OwnerComp.GetAIOwner()->GetPawn<ACoreNpc>();
	if(!Npc)
		return false;

	TArray<EEffectSubTypes> Effects = Npc->GetCurrentEffects();
	return Effects.Contains(EffectSubType);

}