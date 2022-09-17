// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.
using System;
using System.IO;
using UnrealBuildTool;

public class Myproject : ModuleRules
{
	public Myproject(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay" });

        string strPath = Path.GetFullPath(Path.Combine(ModuleDirectory, "ThirdParty/lib/Win64/"));

        PublicLibraryPaths.Add(strPath);

       // PublicAdditionalLibraries.Add("libboost_system-vc140-mt-1_57.lib");
	}
}
