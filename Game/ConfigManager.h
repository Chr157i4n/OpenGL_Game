#pragma once
#include "defines.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <string>

#include "Logger.h"


enum ShadowOption {
	off,
	hard,
	soft,
};

enum FullscreenOption {
	windowed,
	windowed_borderless,
	fullscreen,
};

static class ConfigManager
{

public:

	static void init(std::string nConfigFileName);

	static std::string readConfig(std::string key);

	static void readAllConfigs();

	static void writeConfig(std::string key, std::string value);

	static void writeAllConfigs();

	//SETTINGS
	//[General]
	static std::string level;
	static int bots;
	static float bot_speed_mult;
	static bool show_debug_console;
	static float mouse_sensitivity;
	static float music_volume;
	static int max_bullets;

	//[Renderer]
	static int fullscreen_resolution_width;
	static int fullscreen_resolution_height;
	static int windowed_resolution_width;
	static int windowed_resolution_height;
	//[Renderer - Reflection]
	static int env_map_resolution;
	static int env_map_render_interval;
	static bool env_map_render_characters;
	//[Renderer - Shadows]
	static ShadowOption shadow_option;
	static int shadow_map_resolution;
	static int fov;
	static FullscreenOption fullscreen_option;
	static int fps_limit_ingame;
	static int fps_limit_menu;
	static bool v_sync;
	
	

private:

	static size_t split(const std::string& txt, std::vector<std::string>& strs, char ch);

	static std::string trim(std::string& str);

	static std::string configFileName;


};

