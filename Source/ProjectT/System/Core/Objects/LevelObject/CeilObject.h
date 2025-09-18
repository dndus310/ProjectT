// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CoreObject.h"
#include "CeilObject.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTT_API ACeilObject : public ACoreObject
{
	GENERATED_BODY()
	
public:	
	void ActivateCeil(bool bActivate);

protected:
	virtual void ShowVisual_Implementation() override;
	virtual void HideVisual_Implementation() override;

public:

};
