// Fill out your copyright notice in the Description page of Project Settings.


#include "CeilObject.h"

void ACeilObject::ActivateCeil(bool bActivate)
{
	bActivate ? IVisualizable::Execute_ShowVisual(this) : IVisualizable::HideVisual_Implementation();
}

void ACeilObject::ShowVisual_Implementation()
{
	Super::ShowVisual_Implementation();
}

void ACeilObject::HideVisual_Implementation()
{
	Super::HideVisual_Implementation();
}
