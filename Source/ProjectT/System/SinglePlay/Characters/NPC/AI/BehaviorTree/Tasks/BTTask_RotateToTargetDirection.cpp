// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_RotateToTargetDirection.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType.h"
#include "Kismet/KismetMathLibrary.h"
#include "ProjectT/System/Core/Characters/NPC/AI/Blackboard/BlackboardKeyLibrary.h"
#include "ProjectT/System/SinglePlay/Characters/NPC/SPNpcAgent.h"
#include "ProjectT/System/SinglePlay/Components/SPNpcStatusComponent.h"

UBTTask_RotateToTargetDirection::UBTTask_RotateToTargetDirection() 
{
	NodeName = "Rotate To Target Direction";
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_RotateToTargetDirection::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if(!OwnerComp.GetAIOwner())
		return EBTNodeResult::Failed;
	
	FBTRotateToTargetDirMemory* Memory = CastInstanceNodeMemory<FBTRotateToTargetDirMemory>(NodeMemory);
	Memory->AgentNpc = Cast<ASPNpcAgent>(OwnerComp.GetAIOwner()->GetPawn());
	if(!Memory->AgentNpc.IsValid())
		return EBTNodeResult::Failed;

	UpdateTargetRotation(OwnerComp, NodeMemory);
	
	Memory->bIsAtTargetRotation = FMath::IsNearlyEqual(Memory->AgentNpc->GetActorRotation().Yaw, Memory->TargetRotation.Yaw, 5.f);

	return EBTNodeResult::InProgress;
}

void UBTTask_RotateToTargetDirection::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	FBTRotateToTargetDirMemory* Memory = CastInstanceNodeMemory<FBTRotateToTargetDirMemory>(NodeMemory);
	if(!Memory->AgentNpc.IsValid())
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);

	USPNpcStatusComponent* NpcStatusComp = Memory->AgentNpc->GetNpcStatusComponent();
	if(!NpcStatusComp)
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);

	if(NpcStatusComp->IsDead())
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
	
	if(FMath::IsNearlyEqual(Memory->AgentNpc->GetActorRotation().Yaw, Memory->TargetRotation.Yaw, 5.f))
	{
		Memory->AgentNpc->SetActorRotation(Memory->TargetRotation);
		
		if(Memory->AgentNpc->CanRotateWhileAttack())
		{
			if(!Memory->bIsAtTargetRotation)
				FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		}
		else
		{
			UpdateTargetRotation(OwnerComp, NodeMemory);
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		}
	}
	
	if(Memory->AgentNpc->CanRotateWhileAttack())
		UpdateTargetRotation(OwnerComp, NodeMemory);

	Memory->AgentNpc->SetActorRotation(FMath::RInterpTo(Memory->AgentNpc->GetActorRotation(), Memory->TargetRotation, DeltaSeconds, 5.f));
}

uint16 UBTTask_RotateToTargetDirection::GetInstanceMemorySize() const
{
	return sizeof(FBTRotateToTargetDirMemory);
}

void UBTTask_RotateToTargetDirection::UpdateTargetRotation(UBehaviorTreeComponent& InOwnerComp, uint8* InNodeMemory) const
{
	UBlackboardComponent* BBC = InOwnerComp.GetBlackboardComponent();
	if(!BBC)
		return;

	FBTRotateToTargetDirMemory* Memory = CastInstanceNodeMemory<FBTRotateToTargetDirMemory>(InNodeMemory);
	if(!Memory->AgentNpc.IsValid())
		return;
	
	if(AActor* TargetActor = Cast<AActor>(BBC->GetValueAsObject(UBlackboardKeyLibrary::GetTargetActorKey())))
	{
		Memory->TargetRotation = UKismetMathLibrary::FindLookAtRotation(Memory->AgentNpc->GetActorLocation(), TargetActor->GetActorLocation());
		FVector TargetDir = (TargetActor->GetActorLocation() - Memory->AgentNpc->GetActorLocation()).GetSafeNormal();
		BBC->SetValueAsVector(UBlackboardKeyLibrary::GetTargetDirKey(), TargetDir);
	}
	else
	{
		FVector TargetDir = BBC->GetValueAsVector(UBlackboardKeyLibrary::GetTargetDirKey());
		Memory->TargetRotation = FRotationMatrix::MakeFromX(TargetDir).Rotator();
	}
}
