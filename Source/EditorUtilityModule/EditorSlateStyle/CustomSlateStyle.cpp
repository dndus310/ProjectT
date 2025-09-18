// Fill out your copyright notice in the Description page of Project Settings.
#if WITH_EDITOR

#include "CustomSlateStyle.h"
#include "Slate/SlateGameResources.h"
#include "Styling/SlateStyleRegistry.h"

TSharedPtr< FSlateStyleSet > CustomSlateStyle::StyleInstance = nullptr;

#define IMAGE_BRUSH( RelativePath, ... ) FSlateImageBrush( Style->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )

const FVector2D Icon16x16(16.0f, 16.0f);
const FVector2D Icon20x20(20.0f, 20.0f);
const FVector2D Icon40x40(40.0f, 40.0f);
const FVector2D DebugToolBtn(64.f, 64.f);


void CustomSlateStyle::Initialize()
{
	if(!StyleInstance.IsValid())
	{
	//	StyleInstance = Create();
	//	FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
	}
}

void CustomSlateStyle::Shutdown()
{
//	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
//	ensure(StyleInstance.IsUnique());
//	StyleInstance.Reset();
}

const ISlateStyle& CustomSlateStyle::Get()
{
	return *StyleInstance;
}

FName CustomSlateStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("IGCExtensionStyle"));
	return StyleSetName;
}

//TSharedRef<class FSlateStyleSet> CustomSlateStyle::Create()
//{
//	TSharedRef< FSlateStyleSet > Style = MakeShareable(new FSlateStyleSet("IGCExtensionStyle"));
//
//	/*Style->SetContentRoot(FPaths::EngineContentDir() / TEXT("Editor/Slate"));
//	Style->Set("IGCToolbarIcon.Command2", new IMAGE_BRUSH("Old/Kismet2/compile_512px", DebugToolBtn));
//	Style->Set("IGCToolbarIcon.Command3", new IMAGE_BRUSH("Old/TrashCan", DebugToolBtn));*/
//
//	return Style;
//}

#undef IMAGE_BRUSH

#endif