#include "Menu_Main.h"

#include "Game.h"
#include "UI.h"

Menu_Main::Menu_Main()
{
	pB_start = new UI_Element_Button(10, 130, 200, 50, 0, "Start");
	pB_start->setCallback([&] { Game::startGame(); });
	addMenuElement(pB_start);

	pB_options = new UI_Element_Button(10, 70, 200, 50, 0, "Optionen");
	pB_options->setCallback([&] { Game::toggleSubMenu(MenuType::MENU_OPTIONS); });
	addMenuElement(pB_options);

	pB_exit = new UI_Element_Button(10, 10, 200, 50, 0, "Beenden");
	pB_exit->setCallback([&] { Game::quit(); });
	addMenuElement(pB_exit);
}
