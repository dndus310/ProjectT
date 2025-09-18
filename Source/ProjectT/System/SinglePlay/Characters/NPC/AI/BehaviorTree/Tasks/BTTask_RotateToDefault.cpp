// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_RotateToDefault.h"

#include "AIController.h"
#include "ProjectT/System/SinglePlay/Characters/NPC/SPNpcStationary.h"

UBTTask_RotateToDefault::UBTTask_RotateToDefault()
{
	NodeName = "Rotate To Default";
	bNotifyTick = true;
	bIgnoreRestartSelf = true;
}

EBTNodeResult::Type UBTTask_RotateToDefault::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if(!OwnerComp.GetAIOwner())
		return EBTNodeResult::Failed;
	
	FBTRotateToDefaultMemory* Memory = CastInstanceNodeMemory<FBTRotateToDefaultMemory>(NodeMemory);
	Memory->NpcStationary = Cast<ASPNpcStationary>(OwnerComp.GetAIOwner()->GetPawn());
	if(!Memory->NpcStationary.IsValid())
		return EBTNodeResult::Failed;

	return EBTNodeResult::InProgress;
}

void UBTTask_RotateToDefault::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	FBTRotateToDefaultMemory* Memory = CastInstanceNodeMemory<FBTRotateToDefaultMemory>(NodeMemory);
	if(!Memory->NpcStationary.IsValid())
		return;

	Memory->CurrentRotation = Memory->NpcStationary->GetActorRotation();
	
	float YawDelta = FMath::FindDeltaAngleDegrees(Memory->CurrentRotation.Yaw, Memory->NpcStationary->DefaultRotation.Yaw);
	if (FMath::Abs(YawDelta) < 1.f)
	{
		Memory->NpcStationary->SetActorRotation(FRotator(Memory->CurrentRotation.Pitch, Memory->NpcStationary->DefaultRotation.Yaw, Memory->CurrentRotation.Roll));
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}

	Memory->NpcStationary->SetActorRotation(FMath::RInterpTo(Memory->CurrentRotation, FRotator(Memory->CurrentRotation.Pitch, Memory->NpcStationary->DefaultRotation.Yaw, Memory->CurrentRotation.Roll), DeltaSeconds, 5.f));
}

uint16 UBTTask_RotateToDefault::GetInstanceMemorySize() const
{
	return sizeof(FBTRotateToDefaultMemory);
}
