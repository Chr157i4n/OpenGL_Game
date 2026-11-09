#include "Menu_Pause.h"

#include "Game.h"
#include "UI.h"
#include "Localization.h"

Menu_Pause::Menu_Pause()
{
	setLayout(MenuLayout(1, 200, 50));

	UI_Element_Button* pB_resume = new UI_Element_Button(0, 0, 200, 50, 0, Locale::get("menu.resume"));
	pB_resume->setCallback([&] { Game::toggleMenu(); return MENU_OK; });
	addMenuElement(pB_resume, 0, 0);

	UI_Element_Button* pB_restart = new UI_Element_Button(0, 0, 200, 50, 0, Locale::get("menu.restart"));
	pB_restart->setCallback([&] { Map::restart(); return MENU_OK; });
	addMenuElement(pB_restart, 1, 0);

	UI_Element_Button* pB_options = new UI_Element_Button(0, 0, 200, 50, 0, Locale::get("menu.options"));
	pB_options->setCallback([&] { Game::toggleSubMenu(MenuType::MENU_OPTIONS); return MENU_CONSUMED; });
	addMenuElement(pB_options, 2, 0);

	UI_Element_Button* pB_backtomenu = new UI_Element_Button(0, 0, 200, 50, 0, Locale::get("menu.mainmenu"));
	pB_backtomenu->setCallback([&] { Game::toggleSubMenu(MenuType::MENU_MAIN); return MENU_CONSUMED; });
	addMenuElement(pB_backtomenu, 3, 0);

	UI_Element_Button* pB_exit = new UI_Element_Button(0, 0, 200, 50, 0, Locale::get("menu.exit"));
	pB_exit->setCallback([&] { Game::quit(); return MENU_OK; });
	addMenuElement(pB_exit, 4, 0);
}
