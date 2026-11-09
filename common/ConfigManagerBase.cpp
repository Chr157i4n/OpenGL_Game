#include "ConfigManagerBase.h"

#include <fstream>

std::string			ConfigManagerBase::configFileName = "config.json";
nlohmann::json		ConfigManagerBase::configJson;

//SETTINGS shared by client and server.
std::string			ConfigManagerBase::level = "level_test.json";
int					ConfigManagerBase::bots = 0;
float				ConfigManagerBase::bot_speed_mult = 0.5;
int					ConfigManagerBase::max_bullets = 20;
int					ConfigManagerBase::fps_limit_server = 60;
int					ConfigManagerBase::server_port = 12975;

void ConfigManagerBase::loadConfigFile(const std::string& nConfigFileName)
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
}

std::string ConfigManagerBase::readConfig(const std::string& key)
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

void ConfigManagerBase::saveConfig()
{
	std::ofstream file(configFileName);
	if (!file)
	{
		Logger::log("Error opening configfile for saving: " + configFileName);
		return;
	}

	file << configJson.dump(2) << '\n';
}

void ConfigManagerBase::writeConfig(const std::string& key, bool value)
{
	Logger::log("Writing Configuration key: " + key + " value: " + (value ? "true" : "false"));
	configJson[key] = value;
	saveConfig();
}

void ConfigManagerBase::writeConfig(const std::string& key, int value)
{
	Logger::log("Writing Configuration key: " + key + " value: " + std::to_string(value));
	configJson[key] = value;
	saveConfig();
}

void ConfigManagerBase::writeConfig(const std::string& key, double value)
{
	Logger::log("Writing Configuration key: " + key + " value: " + std::to_string(value));
	configJson[key] = value;
	saveConfig();
}

void ConfigManagerBase::writeConfig(const std::string& key, const std::string& value)
{
	Logger::log("Writing Configuration key: " + key + " value: " + value);

	// The JSON type is derived from the value itself: real booleans become booleans,
	// whole numbers become integers, numbers with a decimal point stay floats, and
	// anything else is stored as a string.
	if (value == "true" || value == "false")
		configJson[key] = (value == "true");
	else if (looksNumeric(value))
	{
		if (value.find('.') == std::string::npos)
			configJson[key] = std::stoll(value);
		else
			configJson[key] = std::stod(value);
	}
	else
		configJson[key] = value;

	saveConfig();
}

void ConfigManagerBase::readCommonConfigs()
{
	Logger::log("Reading common Configuration");
	std::string config = "";

	config = ConfigManagerBase::readConfig("level");
	if (config != "") ConfigManagerBase::level = config;

	config = ConfigManagerBase::readConfig("bots");
	if (config != "") ConfigManagerBase::bots = std::stoi(config);

	config = ConfigManagerBase::readConfig("bot_speed_mult");
	if (config != "") ConfigManagerBase::bot_speed_mult = std::stof(config);

	config = ConfigManagerBase::readConfig("max_bullets");
	if (config != "") ConfigManagerBase::max_bullets = std::stoi(config);

	config = ConfigManagerBase::readConfig("fps_limit_server");
	if (config != "") ConfigManagerBase::fps_limit_server = std::stoi(config);

	config = ConfigManagerBase::readConfig("server_port");
	if (config != "") ConfigManagerBase::server_port = std::stoi(config);
}

void ConfigManagerBase::writeCommonConfigs()
{
	Logger::log("Writing common Configuration");
	ConfigManagerBase::writeConfig("level", ConfigManagerBase::level);
	ConfigManagerBase::writeConfig("bots", ConfigManagerBase::bots);
	ConfigManagerBase::writeConfig("bot_speed_mult", ConfigManagerBase::bot_speed_mult);
	ConfigManagerBase::writeConfig("max_bullets", ConfigManagerBase::max_bullets);
	ConfigManagerBase::writeConfig("fps_limit_server", ConfigManagerBase::fps_limit_server);
	ConfigManagerBase::writeConfig("server_port", ConfigManagerBase::server_port);
}
