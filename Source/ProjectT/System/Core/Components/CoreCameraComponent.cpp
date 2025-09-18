// Fill out your copyright notice in the Description page of Project Settings.


#include "CoreCameraComponent.h"

#include "Components/DrawFrustumComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/SpringArmComponent.h"
#include "ProjectT/System/Core/Interfaces/CoreCameraControl.h"
#include "ProjectT/ProjectT.h"
#include "ProjectT/System/Core/Common/GlobalUtilsLibrary.h"

UCoreCameraComponent::UCoreCameraComponent():
	ZoomLerpCurve(nullptr),
	EnableLag_Location(0),
	EnableLag_Rotation(0),
	LocationLag(10),
	RotationLag(10),
	CameraZoomTimeline(nullptr),
	CameraZoomInOut(),
	CurrentCameraZoom(0),
	ZoomValue(0)
{
	PrimaryComponentTick.bCanEverTick = true;
	CameraZoomTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("CameraZoomTimeline"));
}

void UCoreCameraComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UCoreCameraComponent::BeginPlay()
{
	Super::BeginPlay();

	if (USpringArmComponent* ParentArm = Cast<USpringArmComponent>(GetAttachParent()))
	{
		ParentArm->bEnableCameraLag = EnableLag_Location;
		ParentArm->bEnableCameraRotationLag = EnableLag_Rotation;
		ParentArm->CameraLagSpeed = LocationLag;
		ParentArm->CameraRotationLagSpeed = RotationLag;
	}
}

void UCoreCameraComponent::StartFade(EFadeType FadeType, FOnStaticFinishCameraFade FinishEvent, float Duration, float StartDelay)
{
	APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	NMT_CHECKF(CameraManager);

	if(StartDelay > 0)
	{
		UWorld* World = UGlobalUtilsLibrary::GetValidWorld(this);
		World->GetTimerManager().SetTimer(FadeActionHandle, [this, World, FadeType, CameraManager, Duration, FinishEvent]()
		{
			switch (FadeType)
			{
				case EFadeType::FadeIn:
				{
					CameraManager->StartCameraFade(1.f, 0.f, Duration, FLinearColor(0.f, 0.f, 0.f), false, true);
					break;	
				}
				case EFadeType::FadeOut:
				{
					CameraManager->StartCameraFade(0.f, 1.f, Duration, FLinearColor(0.f, 0.f, 0.f), false, true);
					break;	
				}
				default: NMT_MSG_CHECKF(0, "Does not allow value.");
			}

			FTimerHandle FinishHandle;
			World->GetTimerManager().SetTimer(FinishHandle, FTimerDelegate::CreateLambda([this, FinishEvent]()
			{
				if(FinishEvent.IsBound())
					FinishEvent.Execute();

			}), Duration, false);		
		}, StartDelay, false);
	}
}

void UCoreCameraComponent::StartZoom(EZoomType ZoomType, float ZoomFactor, float Duration)
{
	CurrentCameraZoom = OrthoWidth;
	ZoomValue = ZoomFactor;

	Duration = UKismetMathLibrary::FClamp(Duration, 0.01, Duration);

	if (ZoomLerpCurve != nullptr)
	{
		CameraZoomInOut = ZoomType;

		InterpFunction.BindDynamic(this, &UCoreCameraComponent::CameraZoomUpdate);
		CameraZoomTimeline->AddInterpFloat(ZoomLerpCurve, InterpFunction, FName("doUpdate"));
		CameraZoomTimeline->SetLooping(false);
		CameraZoomTimeline->SetPlayRate(1 / Duration);
		CameraZoomTimeline->SetTimelineLengthMode(TL_LastKeyFrame);
		CameraZoomTimeline->PlayFromStart();
	}
}

void UCoreCameraComponent::CameraFade(EFadeType FadeType, FOnFinishCameraFade FinishEvent, float Duration, float StartDelay)
{
	StartFade(FadeType, FOnStaticFinishCameraFade::CreateLambda([FinishEvent](){
		if(FinishEvent.IsBound())
			FinishEvent.Execute();
	}), Duration, StartDelay);
}

void UCoreCameraComponent::CameraZoom(EZoomType ZoomType, float ZoomFactor, float Duration)
{
	StartZoom(ZoomType, ZoomFactor, Duration);
}

void UCoreCameraComponent::CameraZoomUpdate(float Value)
{
	float NewZoom = 0.f;

	switch (CameraZoomInOut)
	{
		case EZoomType::ZoomIn:
		{
			NewZoom = UKismetMathLibrary::Lerp(CurrentCameraZoom, CurrentCameraZoom - ZoomValue, Value);
			break;
		}
		case EZoomType::ZoomOut:
		{
			NewZoom = UKismetMathLibrary::Lerp(CurrentCameraZoom, CurrentCameraZoom + ZoomValue, Value);
			break;
		}
		default: NMT_MSG_CHECKF(0, "Does not allow value.");
	}

	SetOrthoWidth(NewZoom);
}
