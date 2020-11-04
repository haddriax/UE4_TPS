// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class UE4TPS : ModuleRules
{
	public UE4TPS(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay" });
	}
}
