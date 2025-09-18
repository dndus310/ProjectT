// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "CoreWidgetManager.generated.h"


UCLASS()
class PROJECTT_API UCoreWidgetManager : public UObject
{
	GENERATED_BODY()

public:
	UCoreWidgetManager();
	virtual void InitializeManager(const UObject* WorldObjectContext);
	virtual void ReleaseManager();

protected:
	UPROPERTY()
	TObjectPtr<UObject> OuterObject;
};
