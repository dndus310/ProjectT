// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CoreCameraControl.generated.h"

UENUM(BlueprintType)
enum class EFadeType : uint8
{
	FadeIn = 0,
	FadeOut = 1
};

UENUM(BlueprintType)
enum class EZoomType : uint8
{
	ZoomIn = 0,
	ZoomOut = 1
};

DECLARE_DELEGATE(FOnStaticFinishCameraFade)

UINTERFACE(MinimalAPI, Blueprintable)
class UCoreCameraControl : public UInterface
{
	GENERATED_BODY()
};

class PROJECTT_API ICoreCameraControl
{
	GENERATED_BODY()

public:
	virtual void StartFade(EFadeType FadeType, FOnStaticFinishCameraFade FinishEvent, float Duration, float StartDelay = 1.f) = 0;
	virtual void StartZoom(EZoomType ZoomType, float ZoomFactor, float Duration) = 0;
};
