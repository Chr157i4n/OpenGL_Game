#pragma once

#include "Menu.h"
#include <vector>
#include <string>

class Menu_Main : public Menu
{
public:

	Menu_Main();

	virtual void drawMenu();

	void loadMap(std::string mapname = "");

private:
	UI_Element_Button *pB_exit, *pB_start_singleplayer, * pB_start_multiplayer, *pB_options;
	UI_Element_Dropdown* dD_maps;
	UI_Element_TextEdit* tE_playername;
};

