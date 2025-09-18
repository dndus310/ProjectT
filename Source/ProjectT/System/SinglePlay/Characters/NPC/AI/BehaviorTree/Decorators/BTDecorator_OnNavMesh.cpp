// Fill out your copyright notice in the Description page of Project Settings.


#include "BTDecorator_OnNavMesh.h"

#include "AIController.h"
#include "NavigationSystem.h"
#include "Components/CapsuleComponent.h"
#include "ProjectT/System/Core/Characters/NPC/CoreNpc.h"
#include "ProjectT/System/Core/Characters/NPC/AI/Controllers/AIControllerBase.h"
#include "ProjectT/System/Core/Common/GlobalUtilsLibrary.h"

UBTDecorator_OnNavMesh::UBTDecorator_OnNavMesh()
{
	NodeName = "On NavMesh";
}

bool UBTDecorator_OnNavMesh::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	AAIControllerBase* AIC = Cast<AAIControllerBase>(OwnerComp.GetAIOwner());
	if(!AIC)
		return false;
	
	ACoreNpc* Npc = AIC->GetPawn<ACoreNpc>();
	if(!Npc)
		return false;
	
	UWorld* World = UGlobalUtilsLibrary::GetValidWorld(Npc);
	if(!World)
		return false;

	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(World);
	if(!NavSystem)
		return false;
	
	float CapsuleRadius, CapsuleHalfHeight;
	UCapsuleComponent* CapsuleComponent = Npc->GetCapsuleComponent();
	if(!CapsuleComponent)
		return false;
	
	CapsuleComponent->GetScaledCapsuleSize(CapsuleRadius, CapsuleHalfHeight);
	FVector Extent(CapsuleRadius, CapsuleRadius, CapsuleHalfHeight);
	FNavLocation OutLocation;

	bool bOnNavMesh = NavSystem->ProjectPointToNavigation(Npc->GetActorLocation(), OutLocation, Extent);
	if(bOnNavMesh)
	{
		return true;
	}
	
	NMT_LOG("NOT On Nav Mesh")
	return false;
}
