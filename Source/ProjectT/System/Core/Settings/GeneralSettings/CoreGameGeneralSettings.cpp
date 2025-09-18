// Fill out your copyright notice in the Description page of Project Settings.


#include "CoreGameGeneralSettings.h"

UCoreGameGeneralSettings* UCoreGameGeneralSettings::Get()
{
	static UCoreGameGeneralSettings* Instance;

	if (Instance != nullptr)
	{
		return Instance;
	}
	
	TObjectIterator<UCoreGameGeneralSettings> SettingsIt(RF_NoFlags);
	if (SettingsIt)
	{
		Instance = *SettingsIt;
	}

	return Instance;
}
