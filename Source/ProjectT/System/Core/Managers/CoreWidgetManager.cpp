// Fill out your copyright notice in the Description page of Project Settings.


#include "CoreWidgetManager.h"

UCoreWidgetManager::UCoreWidgetManager() :
	OuterObject(nullptr)
{
}

void UCoreWidgetManager::InitializeManager(const UObject* WorldObjectContext)
{
	OuterObject = const_cast<UObject*>(WorldObjectContext);
}

void UCoreWidgetManager::ReleaseManager()
{
	OuterObject = nullptr;
}
