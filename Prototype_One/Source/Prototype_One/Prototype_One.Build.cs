// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Prototype_One : ModuleRules
{
	public Prototype_One(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "UMG", "AIModule", "NavigationSystem", "GameplayAbilities", "GameplayTasks", "HTTP", "JsonUtilities", "Json", "Niagara"});
	}
}
