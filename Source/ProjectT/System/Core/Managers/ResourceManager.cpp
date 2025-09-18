// Fill out your copyright notice in the Description page of Project Settings.


#include "ResourceManager.h"

UResourceManager::UResourceManager() :
	OuterObject(nullptr)
{
	
}

void UResourceManager::Initialize(const UObject* WorldObjectContext)
{
	OuterObject = const_cast<UObject*>(WorldObjectContext);
}

void UResourceManager::ReleaseManager()
{
	if(OnPreLoadAssetComplete.IsBound()) OnPreLoadAssetComplete.Unbind();
	
	for(TPair<FSoftObjectPath, TObjectPtr<UObject>> Pair : Resources)
		Pair.Value = nullptr;

	Resources.Empty();

	OuterObject = nullptr;
}

void UResourceManager::RequestPreLoad(const FOnPreLoadAssetComplete& Callback)
{
	if(OnPreLoadAssetComplete.IsBound()) return;
	
	OnPreLoadAssetComplete = Callback;

	
	
}
