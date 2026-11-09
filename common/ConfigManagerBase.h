#pragma once
#include "defines.h"

#include <string>

#include <nlohmann/json.hpp>

#include "Logger.h"

// JSON configuration plumbing shared by the game client and the dedicated server.
// Target-specific settings are handled by the derived ConfigManager classes in
// Game/ConfigManager.h and GameServer/ConfigManager.h.
class ConfigManagerBase
{
public:

	// Reads configJson from the given file. Does not populate the typed settings.
	static void loadConfigFile(const std::string& nConfigFileName);

	static std::string readConfig(const std::string& key);

	static void writeConfig(const std::string& key, const std::string& value);
	static void writeConfig(const std::string& key, bool value);
	static void writeConfig(const std::string& key, int value);
	static void writeConfig(const std::string& key, double value);

	//SETTINGS shared by client and server.
	static std::string level;
	static int bots;
	static float bot_speed_mult;
	static int max_bullets;
	static int fps_limit_server;
	static int server_port;

	// Reads / writes the settings that both the client and the server use.
	static void readCommonConfigs();
	static void writeCommonConfigs();

protected:

	static std::string configFileName;
	static nlohmann::json configJson;

	static void saveConfig();
};
