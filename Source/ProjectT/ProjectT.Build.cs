// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ProjectT : ModuleRules
{
	public ProjectT(ReadOnlyTargetRules Target) : base(Target)
	{
		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core", 
			"CoreUObject", 
			"Engine", 
			"InputCore", 
			"EnhancedInput", 
			"LevelSequence", 
			"MovieScene", 
			"Slate", 
			"SlateCore",
			"UMG", 
			"Niagara",
			"AIModule",
			"ProceduralMeshComponent",
			"NavigationSystem",
			"MoviePlayer"
		});
		if (Target.bBuildEditor)
		{
			PrivateDependencyModuleNames.AddRange(new string[] {"UnrealEd","MaterialEditor", "LevelEditor" , "EditorUtilityModule"});
		}
		else
		{
			PrivateDependencyModuleNames.AddRange(new string[] { });
		}

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
