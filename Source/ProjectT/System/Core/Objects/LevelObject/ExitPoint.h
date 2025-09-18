#pragma once

#include "CoreMinimal.h"
#include "CoreInteractionObject.h"
#include "ExitPoint.generated.h"

DECLARE_DELEGATE(FOnCompleteExitPoint)
UCLASS()
class PROJECTT_API AExitPoint : public ACoreInteractionObject
{
	GENERATED_BODY()

public:
	AExitPoint();
	virtual void TryInteraction(const FInteractionData InSendData, FOnInteractionSuccess Callback) override;
	virtual void ShowVisual_Implementation() override;
	virtual void HideVisual_Implementation() override;
	
	UFUNCTION(BlueprintNativeEvent)
	void ActivateExitPoint();

	void CompleteExitPoint(const FOnCompleteExitPoint& Callback);
	
protected:
	virtual void BeginPlay() override;
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void StartAction();
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void EndAction();	

private:
	FOnCompleteExitPoint OnCompleteExitPoint;
};
