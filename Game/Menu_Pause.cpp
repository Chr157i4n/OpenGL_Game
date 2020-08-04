#include "Menu_Pause.h"

#include "Game.h"
#include "UI.h"

Menu_Pause::Menu_Pause()
{
	UI_Element_Button* pB_resume = new UI_Element_Button(10, 190, 200, 50, 0, "Weiterspielen");
	pB_resume->setCallback([&] { Game::toggleMenu(); });
	addMenuElement(pB_resume);

	UI_Element_Button* pB_restart = new UI_Element_Button(10, 130, 200, 50, 0, "Neustart");
	pB_restart->setCallback([&] { Map::restart(); });
	addMenuElement(pB_restart);

	UI_Element_Button* pB_options = new UI_Element_Button(10, 70, 200, 50, 0, "Optionen");
	pB_options->setCallback([&] { Game::toggleMenuOptions(); });
	addMenuElement(pB_options);

	UI_Element_Button* pB_exit = new UI_Element_Button(10, 10, 200, 50, 0, "Beenden");
	pB_exit->setCallback([&] { Game::quit(); });
	addMenuElement(pB_exit);
}