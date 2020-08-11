#include "ConfigManager.h"

#include "Game.h"


std::string			ConfigManager::configFileName="config.ini";

//SETTINGS
//[General]
std::string			ConfigManager::level="level_test.xml";
int					ConfigManager::bots=0;
float				ConfigManager::bot_speed_mult=0.5;
bool				ConfigManager::show_debug_console=0;
float				ConfigManager::mouse_sensitivity=0.2;
float				ConfigManager::music_volume=0.2;
int					ConfigManager::max_bullets=20;

//[Renderer]
int					ConfigManager::fullscreen_resolution_width=1920;
int					ConfigManager::fullscreen_resolution_height=1080;
int					ConfigManager::windowed_resolution_width=960;
int					ConfigManager::windowed_resolution_height=540;
//[Renderer - Reflection]
int					ConfigManager::env_map_resolution = 512;
int					ConfigManager::env_map_render_interval=0;
bool				ConfigManager::env_map_render_characters;
//[Renderer - Shadows]
ShadowOption		ConfigManager::shadow_option = ShadowOption::soft;
int					ConfigManager::shadow_map_resolution = 4096;

int					ConfigManager::fov=100;
FullscreenOption	ConfigManager::fullscreen_option=FullscreenOption::fullscreen;
int					ConfigManager::fps_limit_ingame=0;
int					ConfigManager::fps_limit_menu=60;
bool				ConfigManager::v_sync=1;


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

		split(line, param, '=');

		for (int i = 0; i < param.size(); i++)
		{
			param[i] = trim(param[i]);
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

config = ConfigManager::readConfig("music_volume");
if (config != "") ConfigManager::music_volume = std::stof(config);

config = ConfigManager::readConfig("max_bullets");
if (config != "") ConfigManager::max_bullets = std::stoi(config);

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
	Logger::log("Writing Configuration key: " + key + " value: " +value);
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

		split(linein, param, '=');

		for (int i = 0; i < param.size(); i++)
		{
			param[i] = trim(param[i]);
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

		if(!filein.eof())
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
	ConfigManager::writeConfig("level", ConfigManager::level);
	ConfigManager::writeConfig("bots", std::to_string(ConfigManager::bots));
	ConfigManager::writeConfig("bot_speed_mult", std::to_string(ConfigManager::bot_speed_mult));
	ConfigManager::writeConfig("show_debug_console", std::to_string(ConfigManager::show_debug_console));
	ConfigManager::writeConfig("mouse_sensitivity", std::to_string(ConfigManager::mouse_sensitivity));
	ConfigManager::writeConfig("music_volume", std::to_string(ConfigManager::music_volume));
	ConfigManager::writeConfig("max_bullets", std::to_string(ConfigManager::max_bullets));
	
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
	ConfigManager::writeConfig("shadow_map_resolution",		std::to_string(ConfigManager::shadow_map_resolution));
	
	
	ConfigManager::writeConfig("fov",			std::to_string(ConfigManager::fov));
	ConfigManager::writeConfig("fullscreen_option", std::to_string(ConfigManager::fullscreen_option));
	ConfigManager::writeConfig("fps_limit_ingame", std::to_string(ConfigManager::fps_limit_ingame));
	ConfigManager::writeConfig("fps_limit_menu", std::to_string(ConfigManager::fps_limit_menu));
	ConfigManager::writeConfig("v_sync", std::to_string(ConfigManager::v_sync));
}

size_t ConfigManager::split(const std::string& txt, std::vector<std::string>& strs, char ch)
	{
		size_t pos = txt.find(ch);
		size_t initialPos = 0;
		strs.clear();

		// Decompose statement
		while (pos != std::string::npos) {
			strs.push_back(txt.substr(initialPos, pos - initialPos));
			initialPos = pos + 1;

			pos = txt.find(ch, initialPos);
		}

		// Add the last one
		strs.push_back(txt.substr(initialPos, (std::min)(pos, txt.size()) - initialPos + 1));

		return strs.size();
	}

std::string ConfigManager::trim(std::string& str)
	{
		size_t first = str.find_first_not_of(' ');
		size_t last = str.find_last_not_of(' ');
		return str.substr(first, (last - first + 1));
	}