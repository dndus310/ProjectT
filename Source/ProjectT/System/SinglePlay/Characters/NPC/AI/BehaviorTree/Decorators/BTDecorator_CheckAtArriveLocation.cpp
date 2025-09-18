// Fill out your copyright notice in the Description page of Project Settings.


#include "BTDecorator_CheckAtArriveLocation.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "Components/CapsuleComponent.h"
#include "ProjectT/System/Core/Characters/NPC/CoreNpc.h"

UBTDecorator_CheckAtArriveLocation::UBTDecorator_CheckAtArriveLocation() : 
	AcceptRadius(1.f),
	bIncludeCapsuleRadius(false)
{
	NodeName = "Check At ArriveLocation";
}

bool UBTDecorator_CheckAtArriveLocation::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	const UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	AAIController* AIC = OwnerComp.GetAIOwner();
	if(!AIC)
		return false;
	
	ACoreNpc* Npc = AIC->GetPawn<ACoreNpc>();
	if(!Npc)
		return false;

	if (BlackboardKey.SelectedKeyType == UBlackboardKeyType_Vector::StaticClass())
	{
		const FVector TargetLocation = Blackboard->GetValue<UBlackboardKeyType_Vector>(BlackboardKey.GetSelectedKeyID());
		FVector2D StartLocation2D = FVector2D(Npc->GetActorLocation().X, Npc->GetActorLocation().Y);
		FVector2D TargetLocation2D = FVector2D(TargetLocation.X, TargetLocation.Y);
		float Distance2D = FVector2D::Distance(StartLocation2D, TargetLocation2D);
		
		if(bIncludeCapsuleRadius)
		{
			UCapsuleComponent* CapsuleComponent = Npc->GetCapsuleComponent();
			if(!CapsuleComponent)
				return Distance2D > AcceptRadius;
			
			return Distance2D > AcceptRadius + CapsuleComponent->GetScaledCapsuleRadius();
		}
		
		return Distance2D > AcceptRadius;
	}
	
	return false;
}
