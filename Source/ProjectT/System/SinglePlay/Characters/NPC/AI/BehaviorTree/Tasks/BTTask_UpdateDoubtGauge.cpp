// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_UpdateDoubtGauge.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "ProjectT/System/Core/Characters/NPC/CoreNpc.h"
#include "ProjectT/System/SinglePlay/Components/SPNpcStatusComponent.h"

UBTTask_UpdateDoubtGauge::UBTTask_UpdateDoubtGauge()
{
	NodeName = TEXT("UpdateDoubtGauge");
	bNotifyTick = true;
	bIgnoreRestartSelf = true;
}

EBTNodeResult::Type UBTTask_UpdateDoubtGauge::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if(!OwnerComp.GetAIOwner())
		return EBTNodeResult::Failed;
	
	ACoreNpc* Npc = Cast<ACoreNpc>(OwnerComp.GetAIOwner()->GetPawn());
	if(!Npc)
		return EBTNodeResult::Failed;
	
	USPNpcStatusComponent* NpcStatusComp = Npc->GetNpcStatusComponent();
	if(!NpcStatusComp)
		return EBTNodeResult::Failed;

	FBTUpdateDoubtGaugeMemory* Memory = CastInstanceNodeMemory<FBTUpdateDoubtGaugeMemory>(NodeMemory);
	
	Memory->CurrentDoubtTime = NpcStatusComp->GetStatusNpcDoubtTime();
	Memory->MaxDoubtTime = NpcStatusComp->GetStatusPreviousNpcDoubtTime();
	Memory->DoubtState = Npc->GetCurrentDoubtState();
	
	return EBTNodeResult::InProgress;
}

void UBTTask_UpdateDoubtGauge::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	ACoreNpc* Npc = Cast<ACoreNpc>(OwnerComp.GetAIOwner()->GetPawn());
	NMT_CHECKF(Npc);
	
	FBTUpdateDoubtGaugeMemory* Memory = CastInstanceNodeMemory<FBTUpdateDoubtGaugeMemory>(NodeMemory);
	switch(Memory->DoubtState)
	{
		case EDoubtState::None:
		{
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
			break;
		}
		case EDoubtState::Increasing:
		{
			Memory->CurrentDoubtTime = FMath::Clamp(Memory->CurrentDoubtTime + DeltaSeconds, 0.f, Memory->MaxDoubtTime);
			Npc->UpdateDoubtTime(Memory->CurrentDoubtTime);
			break;
		}
		case EDoubtState::Decreasing:
		{
			if(Memory->CurrentDoubtTime <= 0.f)
			{
				Npc->ChangeDoubtState(EDoubtState::None);
				FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
			}
			
			Memory->CurrentDoubtTime = FMath::Clamp(Memory->CurrentDoubtTime - (DeltaSeconds / 6.f), 0.f, Memory->MaxDoubtTime);
			Npc->UpdateDoubtTime(Memory->CurrentDoubtTime);
			break;
		}
		default: NOT_IMPLEMENTED()
	}
}

uint16 UBTTask_UpdateDoubtGauge::GetInstanceMemorySize() const
{
	return sizeof(FBTUpdateDoubtGaugeMemory);
}
