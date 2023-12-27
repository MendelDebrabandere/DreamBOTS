// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class DreamBots : ModuleRules
{
	public DreamBots(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "EnhancedInput", "AkAudio", "Niagara", "LevelSequence", "MovieSceneTracks", "MovieScene" });
	}
}
