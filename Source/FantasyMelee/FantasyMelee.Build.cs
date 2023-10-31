// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class FantasyMelee : ModuleRules
{
	public FantasyMelee(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[]
		{
			"CommonInput", "CommonUI", "Core", "CoreUObject", "Engine", "EnhancedInput", "GameplayAbilities", "InputCore"
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"AIModule", "AnimGraphRuntime", "GameplayTags", "GameplayTasks", "MotionWarping"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
