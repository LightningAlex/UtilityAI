using UnrealBuildTool;

public class UtilityAI : ModuleRules
{
	public UtilityAI(ReadOnlyTargetRules Target)
		: base(Target)
	{
		PrivatePCHHeaderFile = "Public/UtilityAI.h";

		PCHUsage = PCHUsageMode.UseSharedPCHs;

		PrivateIncludePaths.Add("UtilityAI/Private");

		PublicDependencyModuleNames.AddRange(
			new string[] {
				"Core",
				"CoreUObject",
				"Engine",
				"AIModule"
			}
		);
	}
}
