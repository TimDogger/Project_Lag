// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Project_Lag : ModuleRules
{
	public Project_Lag(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay" });
	}
}
