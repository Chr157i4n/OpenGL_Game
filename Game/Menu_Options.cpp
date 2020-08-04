#include "Menu_Options.h"

#include "Game.h"
#include "UI.h"

Menu_Options::Menu_Options()
{
	pB_resume = new UI_Element_Button(10, 250, 200, 50, 0, "Test2");
	//pB_resume->setCallback([&] { Game::toggleMenu(); });
	addMenuElement(pB_resume);

	sL_volume = new UI_Element_Slider(10, 190, 200, 50, 0, "Master-Volume");
	sL_volume->setValue(100);
	sL_volume->setCallback([&] { setVolume(); });
	addMenuElement(sL_volume);

	pB_vsync = new UI_Element_Button(10, 130, 200, 50, 0, "V_Sync: An");
	pB_vsync->setCallback([&] { toggleVsync(); });
	addMenuElement(pB_vsync);

	pB_options = new UI_Element_Button(10, 70, 200, 50, 0, "Aufloesung");
	addMenuElement(pB_options);

	pB_back= new UI_Element_Button(10, 10, 200, 50, 0, "Zurueck");
	pB_back->setCallback([&] { Game::toggleMenuOptions(); });
	addMenuElement(pB_back);
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

void Menu_Options::setVolume()
{
	Game::SoundEngine->setSoundVolume(sL_volume->getValue()/100.0);
	Logger::log("changed Volume to: " + std::to_string(sL_volume->getValue()) + " %");
}