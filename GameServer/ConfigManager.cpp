#include "ConfigManager.h"



std::string			ConfigManager::configFileName = "config.json";
nlohmann::json		ConfigManager::configJson;

//SETTINGS
//[General]
std::string			ConfigManager::level = "level_farm.json";
int					ConfigManager::bots = 0;
float				ConfigManager::bot_speed_mult = 0.5;
int					ConfigManager::max_bullets = 20;
int					ConfigManager::fps_limit_server = 60;
//[Network]
int					ConfigManager::server_port= 12975;


void ConfigManager::init(std::string nConfigFileName)
{
	configFileName = nConfigFileName;

	std::ifstream file(configFileName);
	if (file)
	{
		try
		{
			configJson = nlohmann::json::parse(file);
		}
		catch (const std::exception& e)
		{
			Logger::log("Could not parse Configfile: " + configFileName + " - " + e.what());
		}
	}
	else
	{
		Logger::log("Cant open Configfile: " + configFileName);
	}

	ConfigManager::readAllConfigs();
}

std::string ConfigManager::readConfig(std::string key)
{
	Logger::log("reading Configuration key: " + key);

	if (configJson.contains(key))
	{
		const nlohmann::json& value = configJson[key];
		if (value.is_string())
			return value.get<std::string>();
        if (value.is_boolean())
            return value.get<bool>() ? "1" : "0";
        if (value.is_number())
            return value.dump();
	}

	Logger::log("While reading config did not find this key: " + key);
	return "";
}

void ConfigManager::readAllConfigs()
{
	Logger::log("Reading Configuration");
	std::string config = "";

	//[General]
	config = ConfigManager::readConfig("level");
	if (config != "") ConfigManager::level = config;

	config = ConfigManager::readConfig("bots");
	if (config != "") ConfigManager::bots = std::stoi(config);

	config = ConfigManager::readConfig("bot_speed_mult");
	if (config != "") ConfigManager::bot_speed_mult = std::stof(config);

	config = ConfigManager::readConfig("max_bullets");
	if (config != "") ConfigManager::max_bullets = std::stoi(config);

	config = ConfigManager::readConfig("fps_limit_server");
	if (config != "") ConfigManager::fps_limit_server = std::stoi(config);

	//[Network]
	config = ConfigManager::readConfig("server_port");
	if (config != "") ConfigManager::server_port = std::stoi(config);
}

// Stores a value, keeping numbers as numbers and everything else as strings.
static bool looksNumeric(const std::string& value)
{
	if (value.empty())
		return false;

	bool hasDot = false;
	for (char c : value)
	{
		if (c == '.')
		{
			if (hasDot)
				return false;
			hasDot = true;
		}
        else if (c < '0' || c > '9')
		{
			return false;
		}
	}

	return true;
}

void ConfigManager::saveConfig()
{
	std::ofstream file(configFileName);
	if (!file)
	{
		Logger::log("Error opening configfile for saving: " + configFileName);
		return;
	}

	file << configJson.dump(2) << '\n';
}

void ConfigManager::writeConfig(const std::string& key, bool value)
{
	Logger::log("Writing Configuration key: " + key + " value: " + (value ? "true" : "false"));
	configJson[key] = value;
	saveConfig();
}

void ConfigManager::writeConfig(const std::string& key, int value)
{
	Logger::log("Writing Configuration key: " + key + " value: " + std::to_string(value));
	configJson[key] = value;
	saveConfig();
}

void ConfigManager::writeConfig(const std::string& key, double value)
{
	Logger::log("Writing Configuration key: " + key + " value: " + std::to_string(value));
	configJson[key] = value;
	saveConfig();
}

void ConfigManager::writeConfig(const std::string& key, const std::string& value)
{
	Logger::log("Writing Configuration key: " + key + " value: " + value);

	// The JSON type is derived from the value itself: real booleans become booleans,
	// whole numbers become integers, numbers with a decimal point stay floats, and
	// anything else is stored as a string.
	if (value == "true" || value == "false")
		configJson[key] = (value == "true");
	else if (looksNumeric(value))
		configJson[key] = (value.find('.') == std::string::npos) ? std::stoll(value) : std::stod(value);
	else
		configJson[key] = value;

	saveConfig();
}

void ConfigManager::writeAllConfigs()
{
	Logger::log("Writing Configuration");
	//[General]
	ConfigManager::writeConfig("level", ConfigManager::level);
	ConfigManager::writeConfig("bots", ConfigManager::bots);
	ConfigManager::writeConfig("bot_speed_mult", ConfigManager::bot_speed_mult);
	ConfigManager::writeConfig("max_bullets", ConfigManager::max_bullets);
	ConfigManager::writeConfig("fps_limit_server", ConfigManager::fps_limit_server);

	//[Network]
	ConfigManager::writeConfig("server_port", ConfigManager::server_port);
}
