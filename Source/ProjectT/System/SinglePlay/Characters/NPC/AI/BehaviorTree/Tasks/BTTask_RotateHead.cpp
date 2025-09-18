// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_RotateHead.h"

#include "AIController.h"
#include "ProjectT/Data/Npc/NpcDataModel.h"
#include "ProjectT/System/Core/Characters/NPC/AI/Controllers/AIControllerBase.h"
#include "ProjectT/System/SinglePlay/Characters/NPC/SPNpcAgent.h"

UBTTask_RotateHead::UBTTask_RotateHead() :
	HeadRotationType(EHeadRotationType::None)
{
	NodeName = TEXT("RotateHead");
	bNotifyTick = true;
	bIgnoreRestartSelf = true;
}

EBTNodeResult::Type UBTTask_RotateHead::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIControllerBase* AIC = Cast<AAIControllerBase>(OwnerComp.GetAIOwner());
	if (!AIC)
		return EBTNodeResult::Failed;

	FBTRotateHeadMemory* Memory = CastInstanceNodeMemory<FBTRotateHeadMemory>(NodeMemory);
	Memory->Npc = Cast<ACoreNpc>(AIC->GetPawn());
	if(!Memory->Npc.IsValid())
		return EBTNodeResult::Failed;

	switch(HeadRotationType)
	{
		case EHeadRotationType::None:
		{
			Memory->Npc->SetHeadRotationEnable(false);
			return EBTNodeResult::Succeeded;
		}
		case EHeadRotationType::Pause:
		{
			Memory->Npc->ChangeHeadRotationState(HeadRotationType);
			return EBTNodeResult::Succeeded;
		}
		case EHeadRotationType::TurnToTarget:
		{
			if(Memory->Npc->bCanRotateHeadToTarget)
				return EBTNodeResult::Succeeded;
		
			Memory->Npc->bCanRotateHeadToTarget = true;
			AActor* Target = AIC->GetTargetActor();
			if(Target)
				Memory->Npc->SetHeadTargetRotation(Target->GetActorLocation());
			break;
		}
		case EHeadRotationType::Search:
		case EHeadRotationType::Guard:
		case EHeadRotationType::LoopSearch:
			break;
	}

	Memory->Npc->SetHeadRotationEnable(true);
	Memory->Npc->ChangeHeadRotationState(HeadRotationType);

	return EBTNodeResult::InProgress;
}

void UBTTask_RotateHead::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	FBTRotateHeadMemory* Memory = CastInstanceNodeMemory<FBTRotateHeadMemory>(NodeMemory);
	if(!Memory->Npc.IsValid())
		return;

	if(Memory->Npc->bReachedHeadRotation && HeadRotationType != EHeadRotationType::LoopSearch)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}

uint16 UBTTask_RotateHead::GetInstanceMemorySize() const
{
	return sizeof(FBTRotateHeadMemory);
}
