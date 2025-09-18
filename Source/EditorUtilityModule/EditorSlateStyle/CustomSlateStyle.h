// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#if WITH_EDITOR

#include "CoreMinimal.h"

/**
 * 
 */
class EDITORUTILITYMODULE_API CustomSlateStyle
{
public:


	static void Initialize();
	static void Shutdown();
	
	static const ISlateStyle& Get();
	static FName GetStyleSetName();

private:
//	static TSharedRef< class FSlateStyleSet > Create();

private:
	static TSharedPtr< class FSlateStyleSet > StyleInstance;
};

#endif