// Fill out your copyright notice in the Description page of Project Settings.


#include "CoreLoadingScreenSettings.h"

UCoreLoadingScreenSettings::UCoreLoadingScreenSettings():
	bEnableStartUpMovie(false),
	ZOrder(10000.f),
	MinimumLoadingScreenTime(2.f),
	bForceLoadingComplete(0)
{
	CategoryName = TEXT("Game");
}
