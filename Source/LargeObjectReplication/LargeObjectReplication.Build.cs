// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class LargeObjectReplication : ModuleRules {
	public LargeObjectReplication(ReadOnlyTargetRules Target) : base(Target) {
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_3;

		PublicDependencyModuleNames.AddRange(new[] { "Core" });
		PrivateDependencyModuleNames.AddRange(new[]{ "CoreUObject", "Engine" });
	}
}