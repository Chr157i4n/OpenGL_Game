#pragma once
#include "ConfigManagerBase.h"

enum ShadowOption {
	off,
	hard,
	soft,
};

enum FullscreenOption {
	fullscreen,
	windowed,
	windowed_borderless,
};

// Client configuration. Reuses the shared JSON plumbing from ConfigManagerBase and
// adds the settings that only the game client has (audio, renderer, UI language...).
class ConfigManager : public ConfigManagerBase
{

public:

	static void init(std::string nConfigFileName);

	static void readAllConfigs();

	static void writeAllConfigs();

	//SETTINGS
	//[General]
	// UI language, selects locale/<language>.json
	static std::string language;
	static std::string player_name;
	static bool show_debug_console;
	static float mouse_sensitivity;
	//[Network]
	static std::string server_address;
	//[Audio]
	static float master_volume;
	static float music_volume;
	static float effect_volume;
	static float ambient_volume;
	static float voice_volume;
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

};
