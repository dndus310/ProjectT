
#if WITH_EDITOR

#include "EditorUtilityModule.h"
#include "Commands/CustomExtensionCommands.h"
#include "Framework/MultiBox/MultiBoxExtender.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "LevelEditor.h"

IMPLEMENT_MODULE(FEditorUtilityModule, EditorUtilityModule);


#define LOCTEXT_NAMESPACE "IGCMenu"

void FEditorUtilityModule::StartupModule()
{
	CustomSlateStyle::Initialize();

	CustomExtensionCommands::Register();

	IGCCommandList = MakeShareable(new FUICommandList());
	IGCCommandList->MapAction(CustomExtensionCommands::Get().Command1, FExecuteAction::CreateStatic(&FIGCExtensionActions::Action1), FCanExecuteAction());
	IGCCommandList->MapAction(CustomExtensionCommands::Get().Command2, FExecuteAction::CreateStatic(&FIGCExtensionActions::Action2), FCanExecuteAction());
	IGCCommandList->MapAction(CustomExtensionCommands::Get().Command3, FExecuteAction::CreateStatic(&FIGCExtensionActions::Action3), FCanExecuteAction());

	FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");	

	static bool IsCheckedBox1;
	static bool IsCheckedBox2;
	static TArray<TSharedPtr<FString>> ComboboxOptions;
	static TSharedPtr<FString> SelectedOption;

	ComboboxOptions.Add(MakeShareable(new FString("Value1")));
	ComboboxOptions.Add(MakeShareable(new FString("Value2")));
	ComboboxOptions.Add(MakeShareable(new FString("Value3")));

	struct IGCToolbar
	{
		static void CreateIGCToolbar(FToolBarBuilder& ToolbarBuilder)
		{
			ToolbarBuilder.BeginSection("IGCToolbar");
			{	
				ToolbarBuilder.AddToolBarWidget(
					SNew(SButton)			
					.OnClicked_Lambda([]()->FReply { FIGCExtensionActions::Action1(); return FReply::Handled(); })
					[
						SNew(STextBlock)
						.Text(FText::FromString("LevelTool"))
					]					
				);	
			
				ToolbarBuilder.AddToolBarButton(
					CustomExtensionCommands::Get().Command2,
					NAME_None,
					TAttribute<FText>(),
					TAttribute<FText>(),
					FSlateIcon(),						
					NAME_None
				);
				ToolbarBuilder.AddToolBarButton(
					CustomExtensionCommands::Get().Command3,
					NAME_None,
					TAttribute<FText>(),
					TAttribute<FText>(),
					FSlateIcon(),
					NAME_None
				);

				ToolbarBuilder.AddToolBarWidget(
					SNew(SCheckBox)
					.OnCheckStateChanged_Lambda(&FIGCExtensionActions::Action4)
					.IsChecked(IsCheckedBox1), 
					LOCTEXT("SCCheckBox", "CheckBox1")
				);


				ToolbarBuilder.AddToolBarWidget(
					SNew(SCheckBox)
					.OnCheckStateChanged_Lambda(&FIGCExtensionActions::Action5)
					.IsChecked(IsCheckedBox2),
					LOCTEXT("SCCheckBox", "CheckBox2")
				);

				ToolbarBuilder.AddToolBarWidget(
					SNew(SBox)
					.Padding(FMargin(20.f, 0.f, 0.f, 0.f))
					[
						SNew(SComboBox<TSharedPtr<FString>>)
							.OptionsSource(&ComboboxOptions)
							.ToolTipText(LOCTEXT("PlatformComboBox", "Change Build Platform"))
							.OnGenerateWidget_Lambda([](TSharedPtr<FString> Option)
								{
									return SNew(STextBlock).Text(FText::FromString(*Option.Get()));
								})
							.OnSelectionChanged_Lambda([](TSharedPtr<FString> NewSelection, ESelectInfo::Type SelectInfo)
								{
									if (NewSelection.IsValid())
									{
										SelectedOption = NewSelection;
										FIGCExtensionActions::Action6(SelectedOption);
									}
								})
									.InitiallySelectedItem(SelectedOption)
									.Content()
									[
										SNew(STextBlock)
											.Text_Lambda([]()
												{
													return SelectedOption.IsValid() ? FText::FromString(*SelectedOption) : LOCTEXT("NoSelection", "선택없음");
												})
									]
					]
					, LOCTEXT("SCComboBox", "ComboBox"));
			}
			ToolbarBuilder.EndSection();
		}

	};

	TSharedPtr<FExtender> IGCToolbarExtender = MakeShareable(new FExtender());
	IGCToolbarExtender->AddToolBarExtension("Play", EExtensionHook::After, IGCCommandList,
	FToolBarExtensionDelegate::CreateStatic(&IGCToolbar::CreateIGCToolbar));
	LevelEditorModule.GetToolBarExtensibilityManager()->AddExtender(IGCToolbarExtender);
}

void FEditorUtilityModule::ShutdownModule()
{
	CustomExtensionCommands::Unregister();
	CustomSlateStyle::Shutdown();
}

#endif