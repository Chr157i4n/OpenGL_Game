#include "Menu_Options.h"

#include "Game.h"
#include "UI.h"

Menu_Options::Menu_Options()
{
	pB_fullscreen = new UI_Element_Button(10, 250, 200, 50, 0, "Vollbild");
	pB_fullscreen->addState("Vollbild");
	pB_fullscreen->addState("Fenster");
	pB_fullscreen->addState("Randloses Fenster");
	pB_fullscreen->setState(ConfigManager::fullscreen_option);
	pB_fullscreen->setCallback([&] { toggleFullscreen(); return 0; });
	addMenuElement(pB_fullscreen);

	sL_volume_master = new UI_Element_Slider(Game::getWindowWidth()-220, 250, 200, 50, 0, "Master-Volume");
	sL_volume_master->setMinValue(0);
	sL_volume_master->setMaxValue(1);
	sL_volume_master->setValue(ConfigManager::master_volume);
	sL_volume_master->setCallback([&] { setVolume(); return 0; });
	addMenuElement(sL_volume_master);

	sL_volume_music = new UI_Element_Slider(Game::getWindowWidth() - 220, 190, 200, 50, 0, "Musik-Volume");
	sL_volume_music->setMinValue(0);
	sL_volume_music->setMaxValue(1);
	sL_volume_music->setValue(ConfigManager::music_volume);
	sL_volume_music->setCallback([&] { setVolume(); return 0; });
	addMenuElement(sL_volume_music);

	sL_volume_effect = new UI_Element_Slider(Game::getWindowWidth() - 220, 130, 200, 50, 0, "Effekt-Volume");
	sL_volume_effect->setMinValue(0);
	sL_volume_effect->setMaxValue(1);
	sL_volume_effect->setValue(ConfigManager::effect_volume);
	sL_volume_effect->setCallback([&] { setVolume(); return 0; });
	addMenuElement(sL_volume_effect);

	sL_volume_ambient = new UI_Element_Slider(Game::getWindowWidth() - 220, 70, 200, 50, 0, "Umgebungs-Volume");
	sL_volume_ambient->setMinValue(0);
	sL_volume_ambient->setMaxValue(1);
	sL_volume_ambient->setValue(ConfigManager::ambient_volume);
	sL_volume_ambient->setCallback([&] { setVolume(); return 0; });
	addMenuElement(sL_volume_ambient);

	sL_volume_voice = new UI_Element_Slider(Game::getWindowWidth() - 220, 10, 200, 50, 0, "Stimmen-Volume");
	sL_volume_voice->setMinValue(0);
	sL_volume_voice->setMaxValue(1);
	sL_volume_voice->setValue(ConfigManager::voice_volume);
	sL_volume_voice->setCallback([&] { setVolume(); return 0; });
	addMenuElement(sL_volume_voice);

	pB_vsync = new UI_Element_Button(10, 130, 200, 50, 0, "V_Sync: Aus");
	pB_vsync->addState("V_Sync: Aus");
	pB_vsync->addState("V_Sync: An");
	pB_vsync->setState(ConfigManager::v_sync);
	pB_vsync->setCallback([&] { toggleVsync(); return 0; });
	addMenuElement(pB_vsync);

	pB_shadow = new UI_Element_Button(10, 70, 200, 50, 0, "Schatten: Weich");
	pB_shadow->addState("Schatten: Aus");
	pB_shadow->addState("Schatten: Hart");
	pB_shadow->addState("Schatten: Weich");
	pB_shadow->setState(ConfigManager::shadow_option);
	pB_shadow->setCallback([&] { toggleShadows(); return 0; });
	addMenuElement(pB_shadow);

	pB_back= new UI_Element_Button(10, 10, 200, 50, 0, "Zurueck");
	pB_back->setCallback([&] { Game::toggleSubMenu(MenuType::MENU_OPTIONS); return 0; });
	addMenuElement(pB_back);

	sL_shadowres = new UI_Element_Slider(500, Game::getWindowHeight() - 180, 200, 50, 0, "Schatten-Aufloesung");
	sL_shadowres->setMinValue(100);
	sL_shadowres->setMaxValue(10000);
	sL_shadowres->setValue(ConfigManager::shadow_map_resolution);
	sL_shadowres->setCallback([&] { changeShadowMapResolution(); return 0; });
	addMenuElement(sL_shadowres);

	sL_envres = new UI_Element_Slider(500, Game::getWindowHeight() - 120, 200, 50, 0, "Reflektions-Aufloesung");
	sL_envres->setMinValue(100);
	sL_envres->setMaxValue(1000);
	sL_envres->setValue(ConfigManager::env_map_resolution);
	sL_envres->setCallback([&] { changeEnvMapResolution(); return 0; });
	addMenuElement(sL_envres);

	dD_Resolution = new UI_Element_Dropdown(500, Game::getWindowHeight() - 60, 200, 50, 0);
	dD_Resolution->setName("Aufloesung");
	UI_Element_Dropdown_Item* resolution = new UI_Element_Dropdown_Item();
	resolution->label = "20x20";
	resolution->callback = [&] { Game::changeSize(20, 20); Renderer::changeResolution(20, 20); return 0; };
	dD_Resolution->addItem(resolution);
	resolution = new UI_Element_Dropdown_Item();
	resolution->label = "960x540";
	resolution->callback = [&] { Game::changeSize(960, 540); Renderer::changeResolution(960, 540); return 0; };
	dD_Resolution->addItem(resolution);
	resolution = new UI_Element_Dropdown_Item();
	resolution->label = "1008x567";
	resolution->callback = [&] { Game::changeSize(1008, 567); Renderer::changeResolution(1008, 567); return 0; };
	dD_Resolution->addItem(resolution);
	resolution = new UI_Element_Dropdown_Item();
	resolution->label = "1366x768";
	resolution->callback = [&] { Game::changeSize(1366,768); Renderer::changeResolution(1366, 768); return 0; };
	dD_Resolution->addItem(resolution);
	resolution = new UI_Element_Dropdown_Item();
	resolution->label = "1600x900";
	resolution->callback = [&] { Game::changeSize(1600, 900); Renderer::changeResolution(1600, 900); return 0; };
	dD_Resolution->addItem(resolution);
	resolution = new UI_Element_Dropdown_Item();
	resolution->label = "1920x1080";
	resolution->callback = [&] { Game::changeSize(1920, 1080); Renderer::changeResolution(1920, 1080); return 0; };
	dD_Resolution->addItem(resolution);
	resolution = new UI_Element_Dropdown_Item();
	resolution->label = "2560x1440";
	resolution->callback = [&] { Game::changeSize(2560, 1440); Renderer::changeResolution(2560, 1440); return 0; };
	dD_Resolution->addItem(resolution);
	resolution = new UI_Element_Dropdown_Item();
	resolution->label = "3840x2160";
	resolution->callback = [&] { Game::changeSize(3840, 2160); Renderer::changeResolution(3840, 2160); return 0; };
	dD_Resolution->addItem(resolution);
	resolution = new UI_Element_Dropdown_Item();
	resolution->label = "7680x4320";
	resolution->callback = [&] { Game::changeSize(7680, 4320); Renderer::changeResolution(7680, 4320); return 0; };
	dD_Resolution->addItem(resolution);
	addMenuElement(dD_Resolution);
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