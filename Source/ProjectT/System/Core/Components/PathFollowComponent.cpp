// Fill out your copyright notice in the Description page of Project Settings.


#include "PathFollowComponent.h"
#include "Components/SplineComponent.h"
#include "AIController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"


UPathFollowComponent::UPathFollowComponent()
{
	Direction = true;
	SplineComponent = nullptr;
	Destination = FVector::ZeroVector;
	CurrentSplinePoint = 0;
}

void UPathFollowComponent::BeginPlay()
{
	Super::BeginPlay();

	ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
	if (!OwnerCharacter) return;

	AAIController* AIController = Cast<AAIController>(OwnerCharacter->GetController());
	if (!AIController) return;

	SplineComponent = Cast<USplineComponent>(OwnerCharacter->GetComponentByClass(USplineComponent::StaticClass()));

	OwnerController = AIController;
	OwnerController->ReceiveMoveCompleted.AddDynamic(this, &UPathFollowComponent::UpdatePath);	
}

void UPathFollowComponent::UpdatePath(FAIRequestID RequestID, EPathFollowingResult::Type Result)
{
	if (Result == EPathFollowingResult::Success)
	{
		if(ForcedActionMap.Contains(CurrentSplinePoint))
		{
			OnSuccessMove.Broadcast(true, *ForcedActionMap.Find(CurrentSplinePoint));
		}
		else
		{
			OnSuccessMove.Broadcast(false, EPatrolType::Patrol);
		}

		if (Direction)
		{
			if (CurrentSplinePoint == SplineComponent->GetNumberOfSplinePoints() - 1)
			{
				if (SplineComponent->IsClosedLoop())
				{
					CurrentSplinePoint = 0;
					MoveToNextWayPoint();
					return;
				}
				else
				{
					Direction = ~Direction;				
				}
			}
		}
		else
		{
			if (CurrentSplinePoint == 0)
			{
				if (SplineComponent->IsClosedLoop())
				{
					CurrentSplinePoint = SplineComponent->GetNumberOfSplinePoints() - 1;
					MoveToNextWayPoint();
					return;
				}

				else
				{
					Direction = ~Direction;					
				}
			}
		}		
		
		Direction ? CurrentSplinePoint += 1 : CurrentSplinePoint -= 1;
		MoveToNextWayPoint();

	}
}

void UPathFollowComponent::MoveToNextWayPoint(bool ReverseDirection)
{
	if (OwnerController->ReceiveMoveCompleted.IsBound() == false)
	{
		OwnerController->ReceiveMoveCompleted.AddDynamic(this, &UPathFollowComponent::UpdatePath);
	}

	Destination = SplineComponent->GetLocationAtSplinePoint(CurrentSplinePoint, ESplineCoordinateSpace::World);
	OwnerController->MoveTo(Destination);
	ReverseDirection ? Direction = ~Direction : Direction;
}

void UPathFollowComponent::StopMove()
{
	if (OwnerController->ReceiveMoveCompleted.IsBound())
	{
		OwnerController->ReceiveMoveCompleted.Clear();
	}
	OwnerController->StopMovement();
}

void UPathFollowComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UPathFollowComponent::OnComponentDestroyed(bool bDestroyingHierarchy)
{
	Super::OnComponentDestroyed(bDestroyingHierarchy);	
}

