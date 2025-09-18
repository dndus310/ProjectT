// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CoreObjectProvider.generated.h"

UINTERFACE()
class UCoreObjectProvider : public UInterface
{
	GENERATED_BODY()
};

class PROJECTT_API ICoreObjectProvider
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, meta = (Category = "CoreObjectProvider"))
	class UCoreWidgetManager* GetCoreWidgetManager();
};
