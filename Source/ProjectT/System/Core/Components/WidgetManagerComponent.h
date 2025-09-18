// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WidgetManagerComponent.generated.h"


enum class ECommonInputType : uint8;

DECLARE_DYNAMIC_DELEGATE(FOnCompletedCreateWidget);
DECLARE_DYNAMIC_DELEGATE(FOnClosedAllWidget);

UCLASS(ClassGroup="UI", meta=(BlueprintSpawnableComponent))
class PROJECTT_API UWidgetManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UWidgetManagerComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	void AsyncOpenWidget(const TSoftClassPtr<UUserWidget>& InWidgetClass);
	
	UFUNCTION(BlueprintCallable)
	void CloseWidget();

	UFUNCTION(BlueprintCallable)
	void FlushAllWidget(FOnClosedAllWidget InCallBack);
	
	void FlushAllWidget();

	UFUNCTION(BlueprintPure)
	FORCEINLINE UUserWidget* GetActiveWidget() const { return ActiveWidget; };
	
	UFUNCTION(BlueprintCallable)
	void AsyncOpenWidgetWithCallback(const TSoftClassPtr<UUserWidget>& InWidgetClass, FOnCompletedCreateWidget InCallback);

	void ToggleGameUIInput(const bool InbGameAndUI = true) const;

private:
	void OpenWidget(const TSubclassOf<UUserWidget>& InWidgetClass);

public:

	UPROPERTY()
	FOnCompletedCreateWidget OnCompletedCreateWidget;
	UPROPERTY()
	FOnClosedAllWidget OnClosedAllWidget;
	
private:
	UPROPERTY()
	TObjectPtr<UUserWidget> ActiveWidget;
	
	UPROPERTY()
	TArray<TObjectPtr<UUserWidget>> WidgetStack;
};
