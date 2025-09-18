// Fill out your copyright notice in the Description page of Project Settings.
#if WITH_EDITOR

#include "CustomExtensionCommands.h"

//Uncomment if you are necessary

//#include "Misc/MessageDialog.h"
//#include "LevelEditor.h"

//#include "Engine/Selection.h"
//#include "Editor/EditorEngine.h"
#include "Editor/Blutility/Classes/EditorUtilityWidget.h"
#include "EditorUtilitySubsystem.h"
#include "EditorUtilityWidgetBlueprint.h"
//#include "Misc/FileHelper.h"
//#include "HAL/FileManagerGeneric.h"
//#include "NotificationHelper.h"


#define LOCTEXT_NAMESPACE "IGCCommand"

void CustomExtensionCommands::RegisterCommands()
{
	UI_COMMAND(Command1, "LevelTool", "Button1", EUserInterfaceActionType::Button, FInputGesture());
	UI_COMMAND(Command2, "IGCCommand2", "Button2", EUserInterfaceActionType::Button, FInputGesture());
	UI_COMMAND(Command3, "IGCCommand3", "Button3", EUserInterfaceActionType::Button, FInputGesture());
}

void FIGCExtensionActions::Action1()
{	
	UEditorUtilityWidgetBlueprint* LevelToolWBPU = LoadObject<UEditorUtilityWidgetBlueprint>(nullptr, L"/Game/System/Core/Blutilities/WBPU_LevelToolByTable");
	UWorld* World = GEditor->GetEditorWorldContext().World();
	if(World)
	{
		if(UEditorUtilitySubsystem* EditorUtilitySubsystem = GEditor->GetEditorSubsystem<UEditorUtilitySubsystem>())
		{
			if(LevelToolWBPU != nullptr)
			{
				EditorUtilitySubsystem->SpawnAndRegisterTab(LevelToolWBPU);
			}		
		}
	}
}

void FIGCExtensionActions::Action2()
{
	/*Active Action when Press Button2*/
}

void FIGCExtensionActions::Action3()
{
	/*Active Action when Press Button3*/
}

void FIGCExtensionActions::Action4(ECheckBoxState state)
{
	/*Active Action when Checking to CheckBox1*/
}

void FIGCExtensionActions::Action5(ECheckBoxState state)
{
	/*Active Action when Checking to CheckBox2*/
}

void FIGCExtensionActions::Action6(TSharedPtr<FString> NewSelection)
{
	/*Active Action when Selecting to ComboBox's value */
}

#endif