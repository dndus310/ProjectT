// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Mouse.generated.h"

UENUM(BlueprintType)
enum class EMouseResultType: uint8
{
	None = 0 UMETA(DisplayName = "None"),
	Linear UMETA(DisplayName = "Linear"),
	Parabola UMETA(DisplayName = "Parabola")
};

UCLASS()
class PROJECTT_API UMouse : public UObject, public FTickableGameObject
{
	GENERATED_BODY()

public:
	UMouse();
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;
	virtual bool IsTickable() const override;
	void SetTickEnabled(bool InbEnabled);
	void InitializeMouse();
	void OnConstruct();
	void OnEndPlay();

	void AimTracing(bool InbStart, AActor* InStandardActor, const TArray<AActor*>& InAffectActors, const float InRange, const float InAffectRange, const EMouseResultType InResultType, bool InbPathVisible = true);
	FHitResult GetCursorHitResult(AActor* InStandardActor, const TArray<AActor*>& InAffectActors, const float InRange, const EMouseResultType InResultType);

protected:
	FHitResult GetParabolaCalibrateHitResult();
	FHitResult GetLinearCalibrateHitResult();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<class ALinearVisualObject> LinearVisualObject;

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite)
	TSubclassOf<AActor> LinearVisualObjectClass;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<class AParabolaVisualObject> ParabolaVisualObject;

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite)
	TSubclassOf<AActor> ParabolaVisualObjectClass;

private:
	bool bTickEnabled;
	bool bMouseTracing;
	float Range;
	EMouseResultType CurrentResultType;
	ETraceTypeQuery LinearTraceQuery;
	ETraceTypeQuery MouseTraceQuery;
	FHitResult MouseCursorHitResult;
	FHitResult LastUpdatedHitResult;

	UPROPERTY()
	AActor* StandardActor;

	UPROPERTY()
	TArray<AActor*> AffectActors;
	
	UPROPERTY()
	APlayerController* CurrentPlayerController;
};
