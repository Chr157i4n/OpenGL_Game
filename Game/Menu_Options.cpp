#include "Menu_Options.h"

#include "Game.h"
#include "UI.h"

Menu_Options::Menu_Options()
{
	pB_fullscreen = new UI_Element_Button(10, 250, 200, 50, 0, "Vollbild");
	switch (ConfigManager::fullscreenOption)
	{
	case FullscreenOption::fullscreen:
		pB_fullscreen->setLabel("Vollbild");
		break;
	case FullscreenOption::windowed:
		pB_fullscreen->setLabel("Fenster");
		break;
	case FullscreenOption::windowed_borderless:
		pB_fullscreen->setLabel("Randloses Fenster");
		break;
	}
	pB_fullscreen->setCallback([&] { toggleFullscreen(); });
	addMenuElement(pB_fullscreen);

	sL_volume = new UI_Element_Slider(10, 190, 200, 50, 0, "Master-Volume");
	sL_volume->setMinValue(0);
	sL_volume->setMaxValue(1);
	sL_volume->setValue(ConfigManager::musicVolume);
	sL_volume->setCallback([&] { setVolume(); });
	addMenuElement(sL_volume);

	pB_vsync = new UI_Element_Button(10, 130, 200, 50, 0, "V_Sync: An");
	pB_vsync->setCallback([&] { toggleVsync(); });
	addMenuElement(pB_vsync);

	pB_shadow = new UI_Element_Button(10, 70, 200, 50, 0, "Schatten: Weich");
	switch (ConfigManager::shadowOption)
	{
	case ShadowOption::off:
		pB_shadow->setLabel("Schatten: Aus");
		break;
	case ShadowOption::hard:
		pB_shadow->setLabel("Schatten: Hart");
		break;
	case ShadowOption::soft:
		pB_shadow->setLabel("Schatten: Weich");
		break;
	}
	pB_shadow->setCallback([&] { toggleShadows(); });
	addMenuElement(pB_shadow);

	pB_back= new UI_Element_Button(10, 10, 200, 50, 0, "Zurueck");
	pB_back->setCallback([&] { Game::toggleSubMenu(MenuType::MENU_OPTIONS); });
	addMenuElement(pB_back);

	sL_shadowres = new UI_Element_Slider(500, Game::getWindowHeight() - 180, 200, 50, 0, "Schatten-Aufloesung");
	sL_shadowres->setMinValue(100);
	sL_shadowres->setMaxValue(10000);
	sL_shadowres->setValue(ConfigManager::shadowMapResolution);
	sL_shadowres->setCallback([&] { changeShadowMapResolution(); });
	addMenuElement(sL_shadowres);

	sL_envres = new UI_Element_Slider(500, Game::getWindowHeight() - 120, 200, 50, 0, "Reflektions-Aufloesung");
	sL_envres->setMinValue(100);
	sL_envres->setMaxValue(4000);
	sL_envres->setValue(ConfigManager::envMapResolution);
	sL_envres->setCallback([&] { changeEnvMapResolution(); });
	addMenuElement(sL_envres);

	dD_Resolution = new UI_Element_Dropdown(500, Game::getWindowHeight() - 60, 200, 50, 0);
	dD_Resolution->setName("Aufloesung");
	UI_Element_Dropdown_Item* resolution = new UI_Element_Dropdown_Item();
	resolution->label = "20x20";
	resolution->callback = [&] { Game::changeSize(20, 20); Renderer::changeResolution(20, 20); };
	dD_Resolution->addItem(resolution);
	resolution = new UI_Element_Dropdown_Item();
	resolution->label = "960x540";
	resolution->callback = [&] { Game::changeSize(960, 540); Renderer::changeResolution(960, 540); };
	dD_Resolution->addItem(resolution);
	resolution = new UI_Element_Dropdown_Item();
	resolution->label = "1008x567";
	resolution->callback = [&] { Game::changeSize(1008, 567); Renderer::changeResolution(1008, 567);  };
	dD_Resolution->addItem(resolution);
	resolution = new UI_Element_Dropdown_Item();
	resolution->label = "1366x768";
	resolution->callback = [&] { Game::changeSize(1366,768); Renderer::changeResolution(1366, 768);  };
	dD_Resolution->addItem(resolution);
	resolution = new UI_Element_Dropdown_Item();
	resolution->label = "1600x900";
	resolution->callback = [&] { Game::changeSize(1600, 900); Renderer::changeResolution(1600, 900);  };
	dD_Resolution->addItem(resolution);
	resolution = new UI_Element_Dropdown_Item();
	resolution->label = "1920x1080";
	resolution->callback = [&] { Game::changeSize(1920, 1080); Renderer::changeResolution(1920, 1080);  };
	dD_Resolution->addItem(resolution);
	resolution = new UI_Element_Dropdown_Item();
	resolution->label = "2560x1440";
	resolution->callback = [&] { Game::changeSize(2560, 1440); Renderer::changeResolution(2560, 1440);  };
	dD_Resolution->addItem(resolution);
	resolution = new UI_Element_Dropdown_Item();
	resolution->label = "3840x2160";
	resolution->callback = [&] { Game::changeSize(3840, 2160); Renderer::changeResolution(3840, 2160);  };
	dD_Resolution->addItem(resolution);
	resolution = new UI_Element_Dropdown_Item();
	resolution->label = "7680x4320";
	resolution->callback = [&] { Game::changeSize(7680, 4320); Renderer::changeResolution(7680, 4320);  };
	dD_Resolution->addItem(resolution);
	addMenuElement(dD_Resolution);
}

void Menu_Options::toggleVsync()
{
	if (pB_vsync->getLabel() == "V_Sync: An")
	{
		pB_vsync->setLabel("V_Sync: Aus");
		Renderer::enableVSync(false);
	}
	else
	{
		pB_vsync->setLabel("V_Sync: An");
		Renderer::enableVSync(true);
	}
}

void Menu_Options::toggleFullscreen()
{
	if (pB_fullscreen->getLabel() == "Fenster")
	{
		pB_fullscreen->setLabel("Randloses Fenster");
		ConfigManager::fullscreenOption = FullscreenOption::windowed;
	}
	else if (pB_fullscreen->getLabel() == "Randloses Fenster")
	{
		pB_fullscreen->setLabel("Vollbild");
		ConfigManager::fullscreenOption = FullscreenOption::fullscreen;
	}
	else if (pB_fullscreen->getLabel() == "Vollbild")
	{
		pB_fullscreen->setLabel("Fenster");
		ConfigManager::fullscreenOption = FullscreenOption::windowed;
	}

	Game::toggleFullscreen(ConfigManager::fullscreenOption);
}

void Menu_Options::toggleShadows()
{
	if (pB_shadow->getLabel() == "Schatten: Aus")
	{
		pB_shadow->setLabel("Schatten: Hart");
		Renderer::toggleShadows(ShadowOption::hard);
	}
	else if (pB_shadow->getLabel() == "Schatten: Hart")
	{
		pB_shadow->setLabel("Schatten: Weich");
		Renderer::toggleShadows(ShadowOption::soft);
	}
	else if (pB_shadow->getLabel() == "Schatten: Weich")
	{
		pB_shadow->setLabel("Schatten: Aus");
		Renderer::toggleShadows(ShadowOption::off);
	}
}

void Menu_Options::setVolume()
{
	float newVolume = sL_volume->getValue();
	Game::SoundEngine->setSoundVolume(newVolume);
	ConfigManager::musicVolume = newVolume;
	Logger::log("changed Volume to: " + std::to_string(sL_volume->getValue()));
}

void Menu_Options::changeShadowMapResolution()
{
	float newResolution = sL_shadowres->getValue();
	Logger::log("changed Shadow Map Resolution to: " + std::to_string(newResolution) + " px");
	ConfigManager::shadowMapResolution = newResolution;
	Renderer::initFrameBuffer();
}

void Menu_Options::changeEnvMapResolution()
{
	float newResolution = sL_envres->getValue();
	Logger::log("changed Environment Map Resolution to: " + std::to_string(newResolution) + " px");
	ConfigManager::envMapResolution = newResolution;
	Renderer::initFrameBuffer();
	Renderer::resetFrameCount();
}