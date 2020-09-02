#include "ConfigManager.h"

#include "Game.h"


std::string			ConfigManager::configFileName = "config.ini";

//SETTINGS
//[General]
std::string			ConfigManager::player_name = "Player";
std::string			ConfigManager::level = "level_test.xml";
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

	ConfigManager::readAllConfigs();
}

std::string ConfigManager::readConfig(std::string key)
{
	Logger::log("reading Configuration key: " + key);
	std::ifstream configFile;
	configFile.open(configFileName);

	if (!configFile) {
		Logger::log("Cant open Configfile: " + configFileName);
		return "";
	}

	char line[255];

	while (!configFile.eof())
	{
		std::vector<std::string> param;
		configFile.getline(line, 255);


		if (line[0] == '\0' ||	//	line is empty
			line[0] == '#' ||	//	line is comment
			line[0] == ';' ||	//	line is comment
			line[0] == '[')		//	line is section
		{
			continue;
		}



		Helper::split(line, param, '=');

		for (int i = 0; i < param.size(); i++)
		{
			param[i] = Helper::trim(param[i]);
		}

		if (param[0] == key)
		{
			return param[1];
		}

	}

	Logger::log("While reading config did not find this key: " + key);
	return "";
}

void ConfigManager::readAllConfigs()
{
	Logger::log("Reading Configuration");
	std::string config = "";

	//[General]
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

void ConfigManager::writeConfig(std::string key, std::string value)
{
	Logger::log("Writing Configuration key: " + key + " value: " + value);
	std::ifstream filein(configFileName); //File to read from
	std::ofstream fileout("_" + configFileName); //Temporary file
	if (!filein || !fileout)
	{
		Logger::log("Error opening configfiles for saving!");
		return;
	}

	char linein[255];
	std::string lineout;
	bool foundKey = false;

	while (!filein.eof())
	{
		std::vector<std::string> param;
		filein.getline(linein, 255);

		if (linein[0] == '\0' ||	//	line is empty
			linein[0] == '#' ||		//	line is comment
			linein[0] == ';' ||		//	line is comment
			linein[0] == '[')		//	line is section
		{
			lineout = linein;
			fileout << lineout;
			if (!filein.eof())
			{
				fileout << '\n';
			}
			continue;
		}

		Helper::split(linein, param, '=');

		for (int i = 0; i < param.size(); i++)
		{
			param[i] = Helper::trim(param[i]);
		}

		if (param[0] == key)
		{
			lineout = key + " = " + value;
			foundKey = true;
		}
		else
		{
			lineout = linein;
		}

		fileout << lineout;

		if (!filein.eof())
		{
			fileout << '\n';
		}

	}

	if (!foundKey)
	{
		lineout = key + " = " + value;
		fileout << '\n' << lineout;
		Logger::log("While writing did not find this key: " + key + ". Added it at the end");
	}

	filein.close();
	fileout.close();

	std::remove(configFileName.c_str());
	std::rename(("_" + configFileName).c_str(), configFileName.c_str());
}

void ConfigManager::writeAllConfigs()
{
	Logger::log("Writing Configuration");
	//[General]
	ConfigManager::writeConfig("player_name", ConfigManager::player_name);
	ConfigManager::writeConfig("level", ConfigManager::level);
	ConfigManager::writeConfig("bots", std::to_string(ConfigManager::bots));
	ConfigManager::writeConfig("bot_speed_mult", std::to_string(ConfigManager::bot_speed_mult));
	ConfigManager::writeConfig("show_debug_console", std::to_string(ConfigManager::show_debug_console));
	ConfigManager::writeConfig("mouse_sensitivity", std::to_string(ConfigManager::mouse_sensitivity));
	ConfigManager::writeConfig("max_bullets", std::to_string(ConfigManager::max_bullets));

	//[Network]
	ConfigManager::writeConfig("server_address", ConfigManager::server_address);
	ConfigManager::writeConfig("server_port", std::to_string(ConfigManager::server_port));

	//[Audio]
	ConfigManager::writeConfig("master_volume", std::to_string(ConfigManager::master_volume));
	ConfigManager::writeConfig("music_volume", std::to_string(ConfigManager::music_volume));
	ConfigManager::writeConfig("effect_volume", std::to_string(ConfigManager::effect_volume));
	ConfigManager::writeConfig("ambient_volume", std::to_string(ConfigManager::ambient_volume));
	ConfigManager::writeConfig("voice_volume", std::to_string(ConfigManager::voice_volume));

	//[Renderer]
	ConfigManager::writeConfig("fullscreen_resolution_width", std::to_string(ConfigManager::fullscreen_resolution_width));
	ConfigManager::writeConfig("fullscreen_resolution_height", std::to_string(ConfigManager::fullscreen_resolution_height));

	ConfigManager::writeConfig("windowed_resolution_width", std::to_string(ConfigManager::windowed_resolution_width));
	ConfigManager::writeConfig("windowed_resolution_height", std::to_string(ConfigManager::windowed_resolution_height));

	//[Renderer - Reflection]
	ConfigManager::writeConfig("env_map_resolution", std::to_string(ConfigManager::env_map_resolution));
	ConfigManager::writeConfig("env_map_render_interval", std::to_string(ConfigManager::env_map_render_interval));
	ConfigManager::writeConfig("env_map_render_characters", std::to_string(ConfigManager::env_map_render_characters));

	//[Renderer - Shadows]
	ConfigManager::writeConfig("shadow_option", std::to_string(ConfigManager::shadow_option));
	ConfigManager::writeConfig("shadow_map_resolution", std::to_string(ConfigManager::shadow_map_resolution));


	ConfigManager::writeConfig("fov", std::to_string(ConfigManager::fov));
	ConfigManager::writeConfig("fullscreen_option", std::to_string(ConfigManager::fullscreen_option));
	ConfigManager::writeConfig("fps_limit_ingame", std::to_string(ConfigManager::fps_limit_ingame));
	ConfigManager::writeConfig("fps_limit_menu", std::to_string(ConfigManager::fps_limit_menu));
	ConfigManager::writeConfig("v_sync", std::to_string(ConfigManager::v_sync));
}