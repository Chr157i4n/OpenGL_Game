#include "Menu_Multiplayer.h"

#include "Game.h"
#include "UI.h"

Menu_Multiplayer::Menu_Multiplayer()
{
	pB_connect = new UI_Element_Button(10, 130, 200, 50, 0, "Verbinden");
	pB_connect->setCallback([&] { Game::startGame(Game_Mode::GameMode_MultiPlayer); return 2; });
	addMenuElement(pB_connect);

	pB_back = new UI_Element_Button(10, 10, 200, 50, 0, "Zurueck");
	pB_back->setCallback([&] { Game::toggleSubMenu(MenuType::MENU_MULTIPLAYER); return 0; });
	addMenuElement(pB_back);

	tE_address = new UI_Element_TextEdit(500, 70, 200, 50, ConfigManager::server_address, 0);
	tE_address->setFilter("([0-9]|[.])");
	tE_address->setCallback([&] { setServerAddress(); return 0; });
	addMenuElement(tE_address);

	tE_port = new UI_Element_TextEdit(500, 10, 200, 50, std::to_string(ConfigManager::server_port), 0);
	tE_port->setFilter("[0-9]");
	tE_port->setCallback([&] { setServerPort(); return 0; });
	addMenuElement(tE_port);
}
