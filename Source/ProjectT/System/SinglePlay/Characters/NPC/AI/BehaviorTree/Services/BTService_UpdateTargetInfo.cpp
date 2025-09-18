// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_UpdateTargetInfo.h"

#include "AIController.h"
#include "ProjectT/System/Core/Characters/NPC/AI/Controllers/AIControllerBase.h"

UBTService_UpdateTargetInfo::UBTService_UpdateTargetInfo()
{
	NodeName = TEXT("UpdateTargetInfo");
	Interval = 0.5f;
}

void UBTService_UpdateTargetInfo::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AAIControllerBase* AIC = Cast<AAIControllerBase>(OwnerComp.GetAIOwner());
	if(!AIC)
		return;

	AActor* Target = AIC->GetTargetActor();
	if(!Target)
		return;

	if(Target)
	{
		AIC->UpdateTargetLocation(Target->GetActorLocation());
		AIC->UpdateTargetDirection(Target);
	}
}
