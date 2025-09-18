

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ItemObjectControl.generated.h"


UINTERFACE(MinimalAPI, Blueprintable)
class UItemObjectControl : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PROJECTT_API IItemObjectControl
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable,BlueprintNativeEvent)
	FName GetItemRowName();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void ObtainItem();
};
