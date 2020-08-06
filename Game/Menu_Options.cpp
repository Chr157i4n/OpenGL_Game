#include "Menu_Options.h"

#include "Game.h"
#include "UI.h"

Menu_Options::Menu_Options()
{

	sL_volume = new UI_Element_Slider(10, 190, 200, 50, 0, "Master-Volume");
	int musicvolume = std::stoi(ConfigManager::readConfig("musicvolume"));
	sL_volume->setValue(musicvolume);
	sL_volume->setCallback([&] { setVolume(); });
	addMenuElement(sL_volume);

	pB_vsync = new UI_Element_Button(10, 130, 200, 50, 0, "V_Sync: An");
	pB_vsync->setCallback([&] { toggleVsync(); });
	addMenuElement(pB_vsync);

	pB_shadow = new UI_Element_Button(10, 70, 200, 50, 0, "Schatten: Weich");
	pB_shadow->setCallback([&] { toggleShadows(); });
	addMenuElement(pB_shadow);

	pB_back= new UI_Element_Button(10, 10, 200, 50, 0, "Zurueck");
	pB_back->setCallback([&] { Game::toggleMenuOptions(); });
	addMenuElement(pB_back);

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
	Game::SoundEngine->setSoundVolume(sL_volume->getValue()/100.0);
	Logger::log("changed Volume to: " + std::to_string(sL_volume->getValue()) + " %");
}