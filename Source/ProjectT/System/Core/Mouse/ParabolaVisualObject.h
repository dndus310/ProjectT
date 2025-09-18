// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProjectT/System/Core/Interfaces/MouseInteraction.h"
#include "ParabolaVisualObject.generated.h"

UCLASS()
class PROJECTT_API AParabolaVisualObject : public AActor, public IMouseInteraction
{
	GENERATED_BODY()

public:
	AParabolaVisualObject();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void RefreshCursorHitResult_Implementation(FHitResult InHitResult) override;
	void InitializeParabolaObject();
	void SetAimVisibility(bool InbVisible, bool InbPathVisible = true, float InAffectRange = 0.f);

protected:
	FHitResult CursorHitResult;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<USceneComponent> AimSceneComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<UStaticMeshComponent> ParabolaMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<UStaticMeshComponent> AffectAreaMeshComponent;
};
