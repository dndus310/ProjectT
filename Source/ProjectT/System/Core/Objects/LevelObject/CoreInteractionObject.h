// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CoreObject.h"
#include "GameFramework/Actor.h"
#include "ProjectT/System/Core/Interfaces/ObjectInteractionControl.h"
#include "ProjectT/System/Core/Interfaces/Visualizable.h"
#include "CoreInteractionObject.generated.h"


UCLASS()
class PROJECTT_API ACoreInteractionObject : public ACoreObject, public IObjectInteractionControl
{
	GENERATED_BODY()

public:
	ACoreInteractionObject();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void TryInteraction(const FInteractionData InSendData, FOnInteractionSuccess Callback) override;
	virtual void ShowVisual_Implementation() override;
	virtual void HideVisual_Implementation() override;
	virtual void SetVisibility(bool InbVisible);
	void ResetToDefault();
	void StartProgressTime(bool InbStart);
	virtual EInteractionObjectType GetInteractionObjectType_Implementation() override;
	
protected:
	void SetProgressPercent();	

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Interaction, meta = (AllowPrivateAccess = "true"))
	class ACombatCharacter* CurrentOwner;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Interaction, meta = (AllowPrivateAccess = "true"))
	EInteractionObjectType InteractionObjectType;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float SuccessProgressTime;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float CurrentProgressRatio;

private:
	float CurrentProgressTime;
	FTimerHandle ProgressTimerHandle;
};
