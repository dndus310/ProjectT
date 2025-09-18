
#include "ExitPoint.h"
#include "ProjectT/System/Core/Managers/QuestManager.h"

AExitPoint::AExitPoint() :
	OnCompleteExitPoint(nullptr)
{
	
}

void AExitPoint::TryInteraction(const FInteractionData InSendData, FOnInteractionSuccess Callback)
{
	Super::TryInteraction(InSendData, Callback);
}

void AExitPoint::ActivateExitPoint_Implementation()
{
	StartAction();
}

void AExitPoint::StartAction_Implementation()
{
	
}

void AExitPoint::EndAction_Implementation()
{
	if(OnCompleteExitPoint.IsBound())
		OnCompleteExitPoint.Execute();
}

void AExitPoint::ShowVisual_Implementation()
{
}

void AExitPoint::HideVisual_Implementation()
{
}

void AExitPoint::CompleteExitPoint(const FOnCompleteExitPoint& Callback)
{
	if(!Callback.IsBound()) return;
	OnCompleteExitPoint = Callback;
}

void AExitPoint::BeginPlay()
{
	Super::BeginPlay();
}
