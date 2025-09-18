// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "Components/TimelineComponent.h"
#include "Curves/CurveFloat.h"
#include "ProjectT/System/Core/Interfaces/CoreCameraControl.h"
#include "CoreCameraComponent.generated.h"

DECLARE_DYNAMIC_DELEGATE(FOnFinishCameraFade);

UCLASS()
class PROJECTT_API UCoreCameraComponent : public UCameraComponent, public ICoreCameraControl
{
	GENERATED_BODY()

public:
	UCoreCameraComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;	

protected:
	virtual void BeginPlay()override;

public:
	virtual void StartFade(EFadeType FadeType, FOnStaticFinishCameraFade FinishEvent, float Duration, float StartDelay) override;
	virtual void StartZoom(EZoomType ZoomType, float ZoomFactor, float Duration) override;
	
	UFUNCTION(BlueprintCallable, Category = "Design | CameraFade")
	void CameraFade(EFadeType FadeType, FOnFinishCameraFade FinishEvent, float Duration, float StartDelay = 1.f);
	
	UFUNCTION(BlueprintCallable, Category = "Design | CameraZoom")
	void CameraZoom(EZoomType ZoomType, float ZoomFactor, float Duration);

private:
	UFUNCTION()
	void CameraZoomUpdate(float Value);
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Design | CameraZoom")
	UCurveFloat* ZoomLerpCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Design | CameraLag")
	uint8 EnableLag_Location : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Design | CameraLag")
	uint8 EnableLag_Rotation : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Design | CameraLag", meta = (EditCondition = "EnableLag_Location"))
	float LocationLag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Design | CameraLag", meta = (EditCondition = "EnableLag_Rotation"))
	float RotationLag;

	UPROPERTY()
	FOnFinishCameraFade OnFinishCameraFade;

private:
	UPROPERTY()
	UTimelineComponent* CameraZoomTimeline;

	UPROPERTY()
	FTimerHandle FadeActionHandle;
	
	EZoomType CameraZoomInOut;
	float CurrentCameraZoom;
	float ZoomValue;
	FOnTimelineFloat InterpFunction;
};
