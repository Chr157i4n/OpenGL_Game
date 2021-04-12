#pragma once

#include "Menu.h"
#include <vector>
#include <string>

class Menu_MapEditor : public Menu
{
public:

	Menu_MapEditor();

private:
	UI_Element_Button* pB_back, * pB_start;
	UI_Element_TextEdit* tE_mapName;

};

