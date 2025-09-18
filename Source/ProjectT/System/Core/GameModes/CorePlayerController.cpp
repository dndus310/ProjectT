// Fill out your copyright notice in the Description page of Project Settings.


#include "CorePlayerController.h"

#include "Blueprint/UserWidget.h"
#include "ProjectT/System/Core/Characters/Player/CorePlayerCharacter.h"
#include "ProjectT/System/Core/Settings/GameSettings/CoreGameUserSettings.h"
#include "ProjectT/System/Core/Components/WidgetManagerComponent.h"
#include "ProjectT/System/Core/Mouse/Mouse.h"

ACorePlayerController::ACorePlayerController():
	bInitializePossessed(0)
{
	bEnableMouseOverEvents = true;
	bEnableClickEvents = true;

	ClickEventKeys.Add(EKeys::RightMouseButton);
}

void ACorePlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if(!bInitializePossessed)
	{
		if(ACorePlayerCharacter* CPC = Cast<ACorePlayerCharacter>(InPawn))
		{
			USceneComponent* Listener = nullptr;
			CPC->GetAudioListener(Listener);
			NMT_CHECKF(Listener);
			SetAudioListenerOverride(Listener, {}, {});

			Mouse = NewObject<UMouse>(this);
			if(Mouse) Mouse->InitializeMouse();
		}
	}
	bInitializePossessed = true;
}

void ACorePlayerController::OnUnPossess()
{
	Super::OnUnPossess();
	bInitializePossessed = false;
}

void ACorePlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (EndPlayReason == EEndPlayReason::Type::Quit)
	{
		if (WidgetManagerComp->IsValidLowLevel())
		{
			WidgetManagerComp->FlushAllWidget();
		}
	}
}

void ACorePlayerController::InitializePlayerScreen() const
{
	if(NMT_ENSURE(CommonHUD))
	{
		CommonHUD->AddToViewport();
	}	
}

void ACorePlayerController::MouseAimTracing(const bool InbStart, AActor* InStandardActor, const TArray<AActor*>& InAffectActors, const float InRange, const float InAffectRange, const EMouseResultType InResultType, bool InbPathVisible) const
{
	NMT_CHECKF(Mouse);
	Mouse->AimTracing(InbStart, InStandardActor, InAffectActors, InRange, InAffectRange, InResultType, InbPathVisible);
}

FHitResult ACorePlayerController::GetMouseCursorHitResult(AActor* InStandardActor, const TArray<AActor*>& InAffectActors, const float InRange, const EMouseResultType InResultType) const
{
	NMT_CHECKF(Mouse);
	FHitResult LocHitResult;
	if(Mouse != nullptr) LocHitResult = Mouse->GetCursorHitResult(InStandardActor, InAffectActors, InRange, InResultType);
	return LocHitResult;
}

void ACorePlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	//FIXME: 독립형 실행때 강제 스크린적용문제가 발생하여 임시로 주석 처리
	// UCoreGameUserSettings::GetCoreGameUserSettings()->InitializeUserSettings();
	
	FInputModeGameAndUI UIInputMode;
	UIInputMode.SetHideCursorDuringCapture(false);
	SetInputMode(UIInputMode);
	bShowMouseCursor = true;
}

UWidgetManagerComponent* ACorePlayerController::GetWidgetManager()
{
	if (WidgetManagerComp->IsValidLowLevel())
	{
		return WidgetManagerComp;
	}

	return WidgetManagerComp = Cast<UWidgetManagerComponent>(AddComponentByClass(UWidgetManagerComponent::StaticClass(), false, FTransform::Identity, false));
}

UUserWidget* ACorePlayerController::GetCurrentCommonHUD()
{
	NMT_ENSURE(CommonHUD);
	return CommonHUD;
}

