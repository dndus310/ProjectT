// Fill out your copyright notice in the Description page of Project Settings.


#include "ObstacleFilterComponent.h"

#include "ProjectT/System/Core/Characters/Player/CorePlayerCharacter.h"

UObstacleFilterComponent::UObstacleFilterComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UObstacleFilterComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UObstacleFilterComponent::OnObstacleFilterBeginOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(!Cast<ACorePlayerCharacter>(OtherActor))
		return;

	if(OnEnterSphereRange.IsBound())
	{
		OnEnterSphereRange.Broadcast(OtherActor);
	}
}

void UObstacleFilterComponent::OnObstacleFilterEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if(!Cast<ACorePlayerCharacter>(OtherActor))
		return;
	
	if(OnExitedSphereRange.IsBound())
	{
		OnExitedSphereRange.Broadcast();
		NMT_LOG("OnExitedSphereRange");
	}
}

void UObstacleFilterComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UObstacleFilterComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	OnEnterSphereRange.Clear();
	OnExitedSphereRange.Clear();
}

void UObstacleFilterComponent::InitializeProperty(const FObstacleFilterInfo& InObstacleFilterInfo)
{
	ObstacleFilterInfo = InObstacleFilterInfo;
	SetSphereRadius(ObstacleFilterInfo.SphereRadius);

	for(ECollisionChannel ObstacleChannel : ObstacleFilterInfo.TraceCollisionChannels)
	{
		ObjectQueryParams.AddObjectTypesToQuery(ObstacleChannel);
	}

	OnComponentBeginOverlap.AddUniqueDynamic(this, &UObstacleFilterComponent::OnObstacleFilterBeginOverlap);
	if(ObstacleFilterInfo.CheckOutOfRange)
	{
		OnComponentEndOverlap.AddUniqueDynamic(this, &UObstacleFilterComponent::OnObstacleFilterEndOverlap);
	}
}

void UObstacleFilterComponent::ActivateFiltering()
{
	SetCollisionEnabled(ECollisionEnabled::Type::QueryAndPhysics);
	SetComponentTickEnabled(true);
}

void UObstacleFilterComponent::DeactivateFiltering()
{
	if(OnExitedSphereRange.IsBound())
		OnExitedSphereRange.Clear();

	SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	SetComponentTickEnabled(false);
}

TArray<class AActor*> UObstacleFilterComponent::GetNonBlockedActors(const FVector& InStartTrace, const FVector& InEndTrace) const
{
	TArray<AActor*> NonBlockedActors;
	TArray<AActor*> OverlappedActors;

	GetOverlappingActors(OverlappedActors);

	for(AActor* Actor : OverlappedActors)
	{
		if(Actor == GetOwner())
			continue;
		
		FVector EndTraceLocation;
		InEndTrace == FVector::ZeroVector ? EndTraceLocation = Actor->GetActorLocation() : EndTraceLocation = InEndTrace;
		FHitResult HitResult;
		GetWorld()->LineTraceSingleByObjectType(HitResult, InStartTrace, EndTraceLocation, ObjectQueryParams, FCollisionQueryParams());
		FColor HitColor = FColor::Red;
		if(ObstacleFilterInfo.TraceCollisionChannels.IsEmpty())
		{
			HitColor = FColor::Green;
			NonBlockedActors.Add(Actor);
		}
		else
		{
			if(!HitResult.bBlockingHit)
			{
				HitColor = FColor::Green;
				NonBlockedActors.Add(Actor);
			}
		}

#if WITH_EDITOR
	//	DrawDebugLine(GetWorld(), InStartTrace, InEndTrace, HitColor, false, 0.1f, 0, 1.0f);
#endif
	}

	return NonBlockedActors;
}
