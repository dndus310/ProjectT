// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_MoveToNearestNavPoint.h"

#include "AIController.h"
#include "NavigationSystem.h"
#include "Navigation/PathFollowingComponent.h"
#include "ProjectT/System/Core/Characters/NPC/CoreNpc.h"
#include "ProjectT/System/Core/Common/GlobalUtilsLibrary.h"
#include "ProjectT/System/SinglePlay/Components/SPNpcStatusComponent.h"

UBTTask_MoveToNearestNavPoint::UBTTask_MoveToNearestNavPoint() :
	NavSearchExtent2D(300.f)
{
	NodeName = "Move To Nearest NavPoint";
	bCreateNodeInstance = true;
}

EBTNodeResult::Type UBTTask_MoveToNearestNavPoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ACoreNpc* Npc = OwnerComp.GetAIOwner()->GetPawn<ACoreNpc>();
	if(!Npc)
		return EBTNodeResult::Failed;
	
	UWorld* World = UGlobalUtilsLibrary::GetValidWorld(Npc);
	if(!World)
		return EBTNodeResult::Failed;
	
	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(World);
	if(!NavSys)
		return EBTNodeResult::Failed;
	
	CachedOwnerBTComp = &OwnerComp;
	FNavLocation ProjectedLocation = FNavLocation();
	FVector StartPoint = Npc->GetActorLocation();
	NavSys->ProjectPointToNavigation(StartPoint,ProjectedLocation, FVector(NavSearchExtent2D, NavSearchExtent2D, StartPoint.Z));

	if(ProjectedLocation.Location == FVector::ZeroVector)
		return EBTNodeResult::Failed;

	USPNpcStatusComponent* NpcStatus = Npc->GetNpcStatusComponent();
	if(!NpcStatus)
		return EBTNodeResult::Failed;
	
	FVector TargetLocation = FVector(ProjectedLocation.Location.X, ProjectedLocation.Location.Y, StartPoint.Z);
	float Distance = FVector::Dist(StartPoint, TargetLocation);
	float Speed = NpcStatus->GetStatusNpcRun();
	float Duration = FMath::Max(Distance / Speed, 1.f);
	
	LatentInfo.CallbackTarget = this;      
	LatentInfo.ExecutionFunction = FName("OnMoveFinished");
	LatentInfo.Linkage = 0;
	LatentInfo.UUID = 0;              

	UKismetSystemLibrary::MoveComponentTo(Npc->GetRootComponent(),
	 TargetLocation,                   
	 Npc->GetRootComponent()->GetComponentRotation(),         
	 true,                               
	 true,                                
	 Duration,                           
	 false,                             
	 EMoveComponentAction::Type::Move,     
	LatentInfo);
	
	return EBTNodeResult::InProgress;
}

void UBTTask_MoveToNearestNavPoint::OnMoveFinished()
{
	if(CachedOwnerBTComp)
	{
		if(AAIController* AIC = CachedOwnerBTComp->GetAIOwner())
		{
			if(UPathFollowingComponent* PathFollowingComp = AIC->GetPathFollowingComponent())
			{
				PathFollowingComp->Cleanup();
			}
		}
		
		FinishLatentTask(*CachedOwnerBTComp, EBTNodeResult::Succeeded);
	}
}
