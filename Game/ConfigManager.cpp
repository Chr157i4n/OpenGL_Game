#include "ConfigManager.h"

#include "Game.h"

//SETTINGS
//[General]
std::string			ConfigManager::language = "de";
std::string			ConfigManager::player_name = "Player";
bool				ConfigManager::show_debug_console = 0;
float				ConfigManager::mouse_sensitivity = 0.2;
//[Network]
std::string			ConfigManager::server_address = "127.0.0.1";
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
bool				ConfigManager::env_map_render_characters = 0;
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
	ConfigManagerBase::loadConfigFile(nConfigFileName);
	ConfigManager::readAllConfigs();
}

void ConfigManager::readAllConfigs()
{
	Logger::log("Reading Configuration");

	// Shared client/server settings.
	ConfigManagerBase::readCommonConfigs();

	std::string config = "";

	//[General]
	config = ConfigManager::readConfig("language");
	if (config != "") ConfigManager::language = config;

	config = ConfigManager::readConfig("player_name");
	if (config != "") ConfigManager::player_name = config;

	config = ConfigManager::readConfig("show_debug_console");
	if (config != "") ConfigManager::show_debug_console = std::stoi(config);

	config = ConfigManager::readConfig("mouse_sensitivity");
	if (config != "") ConfigManager::mouse_sensitivity = std::stof(config);

	//[Network]
	config = ConfigManager::readConfig("server_address");
	if (config != "") ConfigManager::server_address = config;

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

void ConfigManager::writeAllConfigs()
{
	Logger::log("Writing Configuration");

	// Shared client/server settings.
	ConfigManagerBase::writeCommonConfigs();

	//[General]
	ConfigManager::writeConfig("language", ConfigManager::language);
	ConfigManager::writeConfig("player_name", ConfigManager::player_name);
	ConfigManager::writeConfig("show_debug_console", ConfigManager::show_debug_console);
	ConfigManager::writeConfig("mouse_sensitivity", ConfigManager::mouse_sensitivity);

	//[Network]
	ConfigManager::writeConfig("server_address", ConfigManager::server_address);

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
