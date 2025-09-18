// Fill out your copyright notice in the Description page of Project Settings.


#include "WidgetManagerComponent.h"
#include "Blueprint/UserWidget.h"
#include "ProjectT/System/Core/Common/GlobalUtilsLibrary.h"

UWidgetManagerComponent::UWidgetManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void UWidgetManagerComponent::BeginPlay()
{
	Super::BeginPlay();


	/*OnClosedAllWidget.AddLambda([this]
	{
		ActiveWidget = nullptr;
		ToggleGameUIInput(false);
	});*/
}

// Called every frame
void UWidgetManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UWidgetManagerComponent::AsyncOpenWidget(const TSoftClassPtr<UUserWidget>& InWidgetClass)
{
	UGlobalUtilsLibrary::AsyncLoadSubClass<UUserWidget>(InWidgetClass, [this](const TSubclassOf<UUserWidget>& LoadedWidgetClass)
	{
		if(LoadedWidgetClass)
		{
			OpenWidget(LoadedWidgetClass);

			if(OnCompletedCreateWidget.IsBound())
			{
				OnCompletedCreateWidget.Execute();
			}
		}
	});
}

void UWidgetManagerComponent::AsyncOpenWidgetWithCallback(const TSoftClassPtr<UUserWidget>& InWidgetClass, FOnCompletedCreateWidget InCallback)
{
	UGlobalUtilsLibrary::AsyncLoadSubClass<UUserWidget>(InWidgetClass, [this, InCallback](const TSubclassOf<UUserWidget>& LoadedWidgetClass)
	{
		if(LoadedWidgetClass)
		{
			OnCompletedCreateWidget = InCallback;
			OpenWidget(LoadedWidgetClass);
			OnCompletedCreateWidget.Execute();
		}
	});
}

void UWidgetManagerComponent::OpenWidget(const TSubclassOf<UUserWidget>& InWidgetClass)
{
	if(!WidgetStack.IsEmpty())
	{
		ActiveWidget->SetVisibility(ESlateVisibility::Collapsed);
	}

	ActiveWidget = CreateWidget<UUserWidget>(GetWorld(), InWidgetClass);
	ActiveWidget->AddToViewport();
	ActiveWidget->SetVisibility(ESlateVisibility::Visible);

	if(WidgetStack.IsEmpty())
	{
		ToggleGameUIInput();
	}

	WidgetStack.Emplace(ActiveWidget);
}

void UWidgetManagerComponent::CloseWidget()
{
	if(WidgetStack.IsEmpty()) return;

	WidgetStack.Last()->RemoveFromParent();
	WidgetStack.Pop();

	if(!WidgetStack.IsEmpty())
	{
		ActiveWidget = WidgetStack.Last();
		ActiveWidget->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		if(OnClosedAllWidget.IsBound())
		{
			OnClosedAllWidget.Execute();
		}
	}
}

void UWidgetManagerComponent::FlushAllWidget(FOnClosedAllWidget InCallBack)
{
	for(UUserWidget* Widget : WidgetStack)
	{
		Widget->RemoveFromParent();
	}

	WidgetStack.Empty();

	OnClosedAllWidget = InCallBack;

	if(OnClosedAllWidget.IsBound())
	{
		OnClosedAllWidget.Execute();
	}
}

void UWidgetManagerComponent::FlushAllWidget()
{
	for(UUserWidget* Widget : WidgetStack)
	{
		Widget->RemoveFromParent();
	}

	WidgetStack.Empty();
}

void UWidgetManagerComponent::ToggleGameUIInput(const bool InbGameAndUI/* = true*/) const
{
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if(!NMT_ENSURE(PC)) return;

	InbGameAndUI ? PC->SetInputMode(FInputModeGameAndUI()) : PC->SetInputMode(FInputModeGameOnly());
}
