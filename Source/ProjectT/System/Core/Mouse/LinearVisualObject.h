// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProjectT/System/Core/Interfaces/MouseInteraction.h"
#include "LinearVisualObject.generated.h"

UCLASS()
class PROJECTT_API ALinearVisualObject : public AActor, public IMouseInteraction
{
	GENERATED_BODY()

public:
	ALinearVisualObject();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void RefreshCursorHitResult_Implementation(FHitResult InHitResult) override;
	void InitializeLinearObject();
	void SetAimVisibility(const bool InbVisible);
	
protected:
	FHitResult CursorHitResult;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<USceneComponent> AimSceneRootComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<USceneComponent> AimSceneComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<USceneComponent> AimTipSceneComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<UStaticMeshComponent> AimMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<UStaticMeshComponent> AimTipMeshComponent;
};
