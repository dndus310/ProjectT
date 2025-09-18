// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CorePlayerController.generated.h"

enum class EMouseResultType : uint8;

UCLASS()
class PROJECTT_API ACorePlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	ACorePlayerController();
	
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;	
	void InitializePlayerScreen() const;

	void MouseAimTracing(const bool InbStart, AActor* InStandardActor, const TArray<AActor*>& InAffectActors, const float InRange, const float InAffectRange, const EMouseResultType InResultType, bool InbPathVisible = true) const;
	FHitResult GetMouseCursorHitResult(AActor* InStandardActor, const TArray<AActor*>& InAffectActors, const float InRange, const EMouseResultType InResultType) const;
	
	UFUNCTION(BlueprintPure)
	class UWidgetManagerComponent* GetWidgetManager();
	
	UFUNCTION(BlueprintPure)
	UUserWidget* GetCurrentCommonHUD();
	
protected:
	virtual void BeginPlay() override;
	
private:	
	UPROPERTY()
	TObjectPtr<UWidgetManagerComponent> WidgetManagerComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	TObjectPtr<UUserWidget> CommonHUD;
	
	UPROPERTY()
	TObjectPtr<class UMouse> Mouse;

	uint8 bInitializePossessed : 1;
};
