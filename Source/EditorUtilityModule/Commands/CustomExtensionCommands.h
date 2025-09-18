// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#if WITH_EDITOR

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "../EditorSlateStyle/CustomSlateStyle.h"


/**
 * 
 */
class EDITORUTILITYMODULE_API CustomExtensionCommands : public TCommands<CustomExtensionCommands>
{
public:


	CustomExtensionCommands()
		:TCommands<CustomExtensionCommands>(TEXT("CustomExtensionCommands"), NSLOCTEXT("CustomExtensionCommands", "CustomExtensionCommands", "CustomExtensionCommands"), NAME_None, CustomSlateStyle::GetStyleSetName())
	{

	}

	virtual void RegisterCommands()override;

public:

	//Slate button commands

	TSharedPtr< FUICommandInfo > Command1;
	TSharedPtr< FUICommandInfo > Command2;
	TSharedPtr< FUICommandInfo > Command3;

};


/*Actions by Slate UI*/
class FIGCExtensionActions
{
public:
	static void Action1();
	static void Action2();
	static void Action3();
	static void Action4(ECheckBoxState state);
	static void Action5(ECheckBoxState state);
	static void Action6(TSharedPtr<FString> NewSelection);
};
	


#endif