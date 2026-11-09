#include "Menu_Options.h"

#include "Game.h"
#include "UI.h"
#include "Localization.h"

Menu_Options::Menu_Options()
{
	setLayout(MenuLayout(3, 200, 50));

	// First column: toggles
	pB_fullscreen = new UI_Element_Button(0, 0, 200, 50, 0, Locale::get("menu.fullscreen"));
	pB_fullscreen->addState(Locale::get("menu.fullscreen.on"));
	pB_fullscreen->addState(Locale::get("menu.fullscreen.off"));
	pB_fullscreen->addState(Locale::get("menu.fullscreen.borderless"));
	pB_fullscreen->setState(ConfigManager::fullscreen_option);
	pB_fullscreen->setCallback([&] { toggleFullscreen(); return MENU_OK; });
	addMenuElement(pB_fullscreen, 0, 0);

	pB_vsync = new UI_Element_Button(0, 0, 200, 50, 0, Locale::get("menu.vsync"));
	pB_vsync->addState(Locale::get("menu.vsync.off"));
	pB_vsync->addState(Locale::get("menu.vsync.on"));
	pB_vsync->setState(ConfigManager::v_sync);
	pB_vsync->setCallback([&] { toggleVsync(); return MENU_OK; });
	addMenuElement(pB_vsync, 1, 0);

	pB_shadow = new UI_Element_Button(0, 0, 200, 50, 0, Locale::get("menu.shadows"));
	pB_shadow->addState(Locale::get("menu.shadows.off"));
	pB_shadow->addState(Locale::get("menu.shadows.hard"));
	pB_shadow->addState(Locale::get("menu.shadows.soft"));
	pB_shadow->setState(ConfigManager::shadow_option);
	pB_shadow->setCallback([&] { toggleShadows(); return MENU_OK; });
	addMenuElement(pB_shadow, 2, 0);

	pB_back = new UI_Element_Button(0, 0, 200, 50, 0, Locale::get("menu.back"));
	pB_back->setCallback([&] { Game::toggleSubMenu(MenuType::MENU_OPTIONS); return MENU_CONSUMED; });
	addMenuElement(pB_back, 3, 0);

	// Second column: audio volumes
	sL_volume_master = new UI_Element_Slider(0, 0, 200, 50, 0, Locale::get("menu.volume.master"));
	sL_volume_master->setMinValue(0);
	sL_volume_master->setMaxValue(1);
	sL_volume_master->setValue(ConfigManager::master_volume);
	sL_volume_master->setCallback([&] { setVolume(); return MENU_OK; });
	addMenuElement(sL_volume_master, 0, 1);

	sL_volume_music = new UI_Element_Slider(0, 0, 200, 50, 0, Locale::get("menu.volume.music"));
	sL_volume_music->setMinValue(0);
	sL_volume_music->setMaxValue(1);
	sL_volume_music->setValue(ConfigManager::music_volume);
	sL_volume_music->setCallback([&] { setVolume(); return MENU_OK; });
	addMenuElement(sL_volume_music, 1, 1);

	sL_volume_effect = new UI_Element_Slider(0, 0, 200, 50, 0, Locale::get("menu.volume.effect"));
	sL_volume_effect->setMinValue(0);
	sL_volume_effect->setMaxValue(1);
	sL_volume_effect->setValue(ConfigManager::effect_volume);
	sL_volume_effect->setCallback([&] { setVolume(); return MENU_OK; });
	addMenuElement(sL_volume_effect, 2, 1);

	sL_volume_ambient = new UI_Element_Slider(0, 0, 200, 50, 0, Locale::get("menu.volume.ambient"));
	sL_volume_ambient->setMinValue(0);
	sL_volume_ambient->setMaxValue(1);
	sL_volume_ambient->setValue(ConfigManager::ambient_volume);
	sL_volume_ambient->setCallback([&] { setVolume(); return MENU_OK; });
	addMenuElement(sL_volume_ambient, 3, 1);

	sL_volume_voice = new UI_Element_Slider(0, 0, 200, 50, 0, Locale::get("menu.volume.voice"));
	sL_volume_voice->setMinValue(0);
	sL_volume_voice->setMaxValue(1);
	sL_volume_voice->setValue(ConfigManager::voice_volume);
	sL_volume_voice->setCallback([&] { setVolume(); return MENU_OK; });
	addMenuElement(sL_volume_voice, 4, 1);

	// Third column: rendering resolutions
	sL_shadowres = new UI_Element_Slider(0, 0, 200, 50, 0, Locale::get("menu.shadowresolution"));
	sL_shadowres->setMinValue(100);
	sL_shadowres->setMaxValue(10000);
	sL_shadowres->setValue(ConfigManager::shadow_map_resolution);
	sL_shadowres->setCallback([&] { changeShadowMapResolution(); return MENU_OK; });
	addMenuElement(sL_shadowres, 0, 2);

	sL_envres = new UI_Element_Slider(0, 0, 200, 50, 0, Locale::get("menu.reflectionresolution"));
	sL_envres->setMinValue(100);
	sL_envres->setMaxValue(1000);
	sL_envres->setValue(ConfigManager::env_map_resolution);
	sL_envres->setCallback([&] { changeEnvMapResolution(); return MENU_OK; });
	addMenuElement(sL_envres, 1, 2);

	dD_Resolution = new UI_Element_Dropdown(0, 0, 200, 50, 0);
	dD_Resolution->setName(Locale::get("menu.resolution"));

	const int resolutions[][2] = {
		{ 20, 20 },
		{ 960, 540 },
		{ 1008, 567 },
		{ 1366, 768 },
		{ 1600, 900 },
		{ 1920, 1080 },
		{ 2560, 1440 },
		{ 3840, 2160 },
		{ 7680, 4320 },
	};

	for (const auto& res : resolutions)
	{
		int width = res[0];
		int height = res[1];

		UI_Element_Dropdown_Item* resolution = new UI_Element_Dropdown_Item();
		resolution->label = std::to_string(width) + "x" + std::to_string(height);
		resolution->callback = [width, height] {
			Game::changeSize(width, height);
            Renderer::changeResolution(width, height);
            return MENU_CONSUMED;
        };
        dD_Resolution->addItem(resolution);
    }
	addMenuElement(dD_Resolution, 2, 2);
}

void Menu_Options::toggleVsync()
{
	int state = pB_vsync->nextState();
	Renderer::toggleVSync(state);
}

void Menu_Options::toggleFullscreen()
{
	int state = pB_fullscreen->nextState();
	Game::toggleFullscreen(static_cast<FullscreenOption>(state));
}

void Menu_Options::toggleShadows()
{
	int state = pB_shadow->nextState();
	Renderer::toggleShadows(static_cast<ShadowOption>(state));
}

void Menu_Options::setVolume()
{
	float newVolume;

	newVolume = sL_volume_master->getValue();
	ConfigManager::master_volume = newVolume;

	newVolume = sL_volume_music->getValue();
	ConfigManager::music_volume = newVolume;

	newVolume = sL_volume_effect->getValue();
	ConfigManager::effect_volume = newVolume;

	newVolume = sL_volume_ambient->getValue();
	ConfigManager::ambient_volume = newVolume;

	newVolume = sL_volume_voice->getValue();
	ConfigManager::voice_volume = newVolume;

	AudioManager::setVolume();
}

void Menu_Options::changeShadowMapResolution()
{
	float newResolution = sL_shadowres->getValue();
	Logger::log("changed Shadow Map Resolution to: " + std::to_string(newResolution) + " px");
	ConfigManager::shadow_map_resolution = newResolution;
	Renderer::initFrameBuffer();
	Renderer::resetFrameCount();
}

void Menu_Options::changeEnvMapResolution()
{
	float newResolution = sL_envres->getValue();
	Logger::log("changed Environment Map Resolution to: " + std::to_string(newResolution) + " px");
	ConfigManager::env_map_resolution = newResolution;
	Renderer::initFrameBuffer();
	Renderer::resetFrameCount();
}
