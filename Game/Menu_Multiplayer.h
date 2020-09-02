#pragma once

#include "Menu.h"
#include <vector>
#include <string>

class Menu_Multiplayer : public Menu
{
public:

	Menu_Multiplayer();

private:
	UI_Element_Button* pB_back, * pB_connect;
	UI_Element_TextEdit* tE_address, *tE_port;

	void setServerAddress()
	{
		ConfigManager::server_address = tE_address->text;
	}

	void setServerPort()
	{
		ConfigManager::server_port = std::stoi(tE_port->text);
	}
};

