// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class EditorUtilityModule : ModuleRules
{
    public EditorUtilityModule(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "Slate", "SlateCore" });


        if (Target.Type == TargetRules.TargetType.Editor)
        {
            PublicDependencyModuleNames.AddRange(new string[] { "UnrealEd", "LevelEditor" });
            PrivateDependencyModuleNames.AddRange(new string[] { "UMG", "Blutility", "UMGEditor" });
        }

    }
}
