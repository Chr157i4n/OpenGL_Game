#pragma once
#include "ConfigManagerBase.h"

// Dedicated server configuration. The server only uses the settings that are
// shared with the client, so this is a thin wrapper around ConfigManagerBase.
class ConfigManager : public ConfigManagerBase
{

public:

	static void init(std::string nConfigFileName);

	static void readAllConfigs();

	static void writeAllConfigs();

};
