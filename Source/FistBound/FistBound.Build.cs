// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class FistBound : ModuleRules
{
	public FistBound(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "UMG", "CommonUI" });

		// MoviePlayer drives the foundation loading screen during level transitions;
		// Slate/SlateCore back the native fallback loading widget.
		PrivateDependencyModuleNames.AddRange(new string[] { "MoviePlayer", "Slate", "SlateCore" });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
