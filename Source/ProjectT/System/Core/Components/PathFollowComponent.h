// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "ProjectT/Data/Npc/NpcDataModel.h"
#include "PathFollowComponent.generated.h"

class AAIController;
class USplineComponent;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnSuccessMove,bool,EPatrolType);


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTT_API UPathFollowComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPathFollowComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void OnComponentDestroyed(bool bDestroyingHierarchy) override;	

	UFUNCTION()
	void UpdatePath(FAIRequestID RequestID, EPathFollowingResult::Type Result);

	UFUNCTION(BlueprintCallable, Category = "Patrol")
	void MoveToNextWayPoint(bool ReverseDirection = false);

	UFUNCTION(BlueprintCallable, Category = "Patrol")
	void StopMove();

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPCAction")
	TMap<int32, EPatrolType> ForcedActionMap;

	FOnSuccessMove OnSuccessMove;

private:
	UPROPERTY()
	TObjectPtr<AAIController> OwnerController;

	UPROPERTY()
	TObjectPtr<USplineComponent> SplineComponent;

	int32 CurrentSplinePoint;
	FVector Destination;
	uint8 Direction : 1;
};
