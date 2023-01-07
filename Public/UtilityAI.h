#pragma once

#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"

/** Module interface for this game's loading screens */
class IUtilityAIModule : public IModuleInterface
{
public:
	/** Loads the module so it can be turned on */
	static inline IUtilityAIModule& Get()
	{
		return FModuleManager::LoadModuleChecked<IUtilityAIModule>("UtilityAI");
	}
};
