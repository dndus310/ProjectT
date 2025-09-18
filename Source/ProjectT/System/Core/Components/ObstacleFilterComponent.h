// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "ObstacleFilterComponent.generated.h"


USTRUCT(Blueprintable)
struct FObstacleFilterInfo
{
	GENERATED_BODY()

	FObstacleFilterInfo() :
		SphereRadius(0.f),
		CheckOutOfRange(false)
	{}

	FObstacleFilterInfo(FName InSphereProfile, float InSphereRadius, bool InCheckOutOfRange) :
		SphereRadius(InSphereRadius),
		CheckOutOfRange(InCheckOutOfRange)
	{}

	UPROPERTY()
	TArray<TEnumAsByte<ECollisionChannel>> TraceCollisionChannels;

	UPROPERTY()
	float SphereRadius;

	UPROPERTY()
	bool CheckOutOfRange;
};


DECLARE_MULTICAST_DELEGATE_OneParam(FOnEnterSphereRange, AActor*);
DECLARE_MULTICAST_DELEGATE(FOnExitedSphereRange);
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTT_API UObstacleFilterComponent : public USphereComponent
{
	GENERATED_BODY()

public:
	UObstacleFilterComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void InitializeProperty(const FObstacleFilterInfo& InObstacleFilterInfo);
	void ActivateFiltering();
	void DeactivateFiltering();
	TArray<class AActor*> GetNonBlockedActors(const FVector& InStartTrace, const FVector& InEndTrace = FVector::ZeroVector) const;
	
protected:
	virtual void BeginPlay() override;
	
	UFUNCTION()
	void OnObstacleFilterBeginOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void OnObstacleFilterEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:
	FOnEnterSphereRange OnEnterSphereRange;
	FOnExitedSphereRange OnExitedSphereRange;

private:
	UPROPERTY()
	FObstacleFilterInfo ObstacleFilterInfo;
	
	FCollisionObjectQueryParams ObjectQueryParams;
};
