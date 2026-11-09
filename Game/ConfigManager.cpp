#include "ConfigManager.h"

#include "Game.h"


std::string			ConfigManager::configFileName = "config.json";
nlohmann::json		ConfigManager::configJson;

//SETTINGS
//[General]
std::string			ConfigManager::language = "de";
std::string			ConfigManager::player_name = "Player";
std::string			ConfigManager::level = "level_test.json";
int					ConfigManager::bots = 0;
float				ConfigManager::bot_speed_mult = 0.5;
bool				ConfigManager::show_debug_console = 0;
float				ConfigManager::mouse_sensitivity = 0.2;
int					ConfigManager::max_bullets = 20;
//[Network]
std::string			ConfigManager::server_address="127.0.0.1";
int					ConfigManager::server_port= 12975;
//[Audio]
float				ConfigManager::master_volume = 0.5;
float				ConfigManager::music_volume = 0.5;
float				ConfigManager::effect_volume = 0.5;
float				ConfigManager::ambient_volume = 0.5;
float				ConfigManager::voice_volume = 0.5;
//[Renderer]
int					ConfigManager::fullscreen_resolution_width = 1920;
int					ConfigManager::fullscreen_resolution_height = 1080;
int					ConfigManager::windowed_resolution_width = 960;
int					ConfigManager::windowed_resolution_height = 540;
//[Renderer - Reflection]
int					ConfigManager::env_map_resolution = 512;
int					ConfigManager::env_map_render_interval = 0;
bool				ConfigManager::env_map_render_characters=0;
//[Renderer - Shadows]
ShadowOption		ConfigManager::shadow_option = ShadowOption::soft;
int					ConfigManager::shadow_map_resolution = 4096;

int					ConfigManager::fov = 100;
FullscreenOption	ConfigManager::fullscreen_option = FullscreenOption::fullscreen;
int					ConfigManager::fps_limit_ingame = 0;
int					ConfigManager::fps_limit_menu = 60;
bool				ConfigManager::v_sync = 1;


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
	config = ConfigManager::readConfig("language");
	if (config != "") ConfigManager::language = config;

	config = ConfigManager::readConfig("player_name");
	if (config != "") ConfigManager::player_name = config;

	config = ConfigManager::readConfig("level");
	if (config != "") ConfigManager::level = config;

	config = ConfigManager::readConfig("bots");
	if (config != "") ConfigManager::bots = std::stoi(config);

	config = ConfigManager::readConfig("bot_speed_mult");
	if (config != "") ConfigManager::bot_speed_mult = std::stof(config);

	config = ConfigManager::readConfig("show_debug_console");
	if (config != "") ConfigManager::show_debug_console = std::stoi(config);

	config = ConfigManager::readConfig("mouse_sensitivity");
	if (config != "") ConfigManager::mouse_sensitivity = std::stof(config);

	config = ConfigManager::readConfig("max_bullets");
	if (config != "") ConfigManager::max_bullets = std::stoi(config);

	//[Network]
	config = ConfigManager::readConfig("server_address");
	if (config != "") ConfigManager::server_address = config;

	config = ConfigManager::readConfig("server_port");
	if (config != "") ConfigManager::server_port = std::stoi(config);

	//[Audio]
	config = ConfigManager::readConfig("master_volume");
	if (config != "") ConfigManager::master_volume = std::stof(config);

	config = ConfigManager::readConfig("music_volume");
	if (config != "") ConfigManager::music_volume = std::stof(config);

	config = ConfigManager::readConfig("effect_volume");
	if (config != "") ConfigManager::effect_volume = std::stof(config);

	config = ConfigManager::readConfig("ambient_volume");
	if (config != "") ConfigManager::ambient_volume = std::stof(config);

	config = ConfigManager::readConfig("voice_volume");
	if (config != "") ConfigManager::voice_volume = std::stof(config);

	//[Renderer]
	config = ConfigManager::readConfig("fullscreen_resolution_width");
	if (config != "") ConfigManager::fullscreen_resolution_width = std::stoi(config);
	config = ConfigManager::readConfig("fullscreen_resolution_height");
	if (config != "") ConfigManager::fullscreen_resolution_height = std::stoi(config);

	config = ConfigManager::readConfig("windowed_resolution_width");
	if (config != "") ConfigManager::windowed_resolution_width = std::stoi(config);
	config = ConfigManager::readConfig("windowed_resolution_height");
	if (config != "") ConfigManager::windowed_resolution_height = std::stoi(config);

	//[Renderer - Reflection]
	config = ConfigManager::readConfig("env_map_render_interval");
	if (config != "") ConfigManager::env_map_render_interval = std::stoi(config);

	config = ConfigManager::readConfig("env_map_render_characters");
	if (config != "") ConfigManager::env_map_render_characters = std::stoi(config);

	config = ConfigManager::readConfig("env_map_resolution");
	if (config != "") ConfigManager::env_map_resolution = std::stoi(config);

	//[Renderer - Shadows]
	config = ConfigManager::readConfig("shadow_option");
	if (config != "") ConfigManager::shadow_option = static_cast<ShadowOption>(std::stoi(config));

	config = ConfigManager::readConfig("shadow_map_resolution");
	if (config != "") ConfigManager::shadow_map_resolution = std::stoi(config);


	config = ConfigManager::readConfig("fov");
	if (config != "") ConfigManager::fov = std::stoi(config);

	config = ConfigManager::readConfig("fullscreen_option");
	if (config != "") ConfigManager::fullscreen_option = static_cast<FullscreenOption>(std::stoi(config));

	config = ConfigManager::readConfig("fps_limit_ingame");
	if (config != "") ConfigManager::fps_limit_ingame = std::stoi(config);

	config = ConfigManager::readConfig("fps_limit_menu");
	if (config != "") ConfigManager::fps_limit_menu = std::stoi(config);

	config = ConfigManager::readConfig("v_sync");
	if (config != "") ConfigManager::v_sync = std::stoi(config);
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
	ConfigManager::writeConfig("language", ConfigManager::language);
	ConfigManager::writeConfig("player_name", ConfigManager::player_name);
	ConfigManager::writeConfig("level", ConfigManager::level);
	ConfigManager::writeConfig("bots", ConfigManager::bots);
	ConfigManager::writeConfig("bot_speed_mult", ConfigManager::bot_speed_mult);
	ConfigManager::writeConfig("show_debug_console", ConfigManager::show_debug_console);
	ConfigManager::writeConfig("mouse_sensitivity", ConfigManager::mouse_sensitivity);
	ConfigManager::writeConfig("max_bullets", ConfigManager::max_bullets);

	//[Network]
	ConfigManager::writeConfig("server_address", ConfigManager::server_address);
	ConfigManager::writeConfig("server_port", ConfigManager::server_port);

	//[Audio]
	ConfigManager::writeConfig("master_volume", ConfigManager::master_volume);
	ConfigManager::writeConfig("music_volume", ConfigManager::music_volume);
	ConfigManager::writeConfig("effect_volume", ConfigManager::effect_volume);
	ConfigManager::writeConfig("ambient_volume", ConfigManager::ambient_volume);
	ConfigManager::writeConfig("voice_volume", ConfigManager::voice_volume);

	//[Renderer]
	ConfigManager::writeConfig("fullscreen_resolution_width", ConfigManager::fullscreen_resolution_width);
	ConfigManager::writeConfig("fullscreen_resolution_height", ConfigManager::fullscreen_resolution_height);

	ConfigManager::writeConfig("windowed_resolution_width", ConfigManager::windowed_resolution_width);
	ConfigManager::writeConfig("windowed_resolution_height", ConfigManager::windowed_resolution_height);

	//[Renderer - Reflection]
	ConfigManager::writeConfig("env_map_resolution", ConfigManager::env_map_resolution);
	ConfigManager::writeConfig("env_map_render_interval", ConfigManager::env_map_render_interval);
	ConfigManager::writeConfig("env_map_render_characters", ConfigManager::env_map_render_characters);

	//[Renderer - Shadows]
	ConfigManager::writeConfig("shadow_option", static_cast<int>(ConfigManager::shadow_option));
	ConfigManager::writeConfig("shadow_map_resolution", ConfigManager::shadow_map_resolution);


	ConfigManager::writeConfig("fov", ConfigManager::fov);
	ConfigManager::writeConfig("fullscreen_option", static_cast<int>(ConfigManager::fullscreen_option));
	ConfigManager::writeConfig("fps_limit_ingame", ConfigManager::fps_limit_ingame);
	ConfigManager::writeConfig("fps_limit_menu", ConfigManager::fps_limit_menu);
	ConfigManager::writeConfig("v_sync", ConfigManager::v_sync);
}
