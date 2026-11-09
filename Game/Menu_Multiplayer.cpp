#include "Menu_Multiplayer.h"

#include "Game.h"
#include "UI.h"
#include "Localization.h"

Menu_Multiplayer::Menu_Multiplayer()
{
	setLayout(MenuLayout(2, 200, 50));

	pB_connect = new UI_Element_Button(0, 0, 200, 50, 0, Locale::get("menu.connect"));
	pB_connect->setCallback([&] { Game::startGame(Game_Mode::GameMode_MultiPlayer); return MENU_CONSUMED; });
	addMenuElement(pB_connect, 0, 0);

	pB_back = new UI_Element_Button(0, 0, 200, 50, 0, Locale::get("menu.back"));
	pB_back->setCallback([&] { Game::toggleSubMenu(MenuType::MENU_MULTIPLAYER); return MENU_CONSUMED; });
	addMenuElement(pB_back, 1, 0);

	tE_address = new UI_Element_TextEdit(0, 0, 200, 50, ConfigManager::server_address, 0);
	tE_address->setFilter("([0-9]|[.])");
	tE_address->setCallback([&] { setServerAddress(); return MENU_OK; });
	addMenuElement(tE_address, 0, 1);

	tE_port = new UI_Element_TextEdit(0, 0, 200, 50, std::to_string(ConfigManager::server_port), 0);
	tE_port->setFilter("[0-9]");
	tE_port->setCallback([&] { setServerPort(); return MENU_OK; });
	addMenuElement(tE_port, 1, 1);
}
