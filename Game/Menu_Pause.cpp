#include "Menu_Pause.h"

#include "Game.h"
#include "UI.h"

Menu_Pause::Menu_Pause()
{
	UI_Element_Button* pB_resume = new UI_Element_Button(10, 250, 200, 50, 0, "Weiterspielen");
	pB_resume->setCallback([&] { Game::toggleMenu(); return 0; });
	addMenuElement(pB_resume);

	UI_Element_Button* pB_restart = new UI_Element_Button(10, 190, 200, 50, 0, "Neustart");
	pB_restart->setCallback([&] { Map::restart(); return 0; });
	addMenuElement(pB_restart);

	UI_Element_Button* pB_options = new UI_Element_Button(10, 130, 200, 50, 0, "Optionen");
	pB_options->setCallback([&] { Game::toggleSubMenu(MenuType::MENU_OPTIONS); return 0; });
	addMenuElement(pB_options);

	UI_Element_Button* pB_backtomenu = new UI_Element_Button(10, 70, 200, 50, 0, "Hauptmenu");
	pB_backtomenu->setCallback([&] { Game::toggleSubMenu(MenuType::MENU_MAIN); return 0; });
	addMenuElement(pB_backtomenu);

	UI_Element_Button* pB_exit = new UI_Element_Button(10, 10, 200, 50, 0, "Beenden");
	pB_exit->setCallback([&] { Game::quit(); return 0; });
	addMenuElement(pB_exit);
}