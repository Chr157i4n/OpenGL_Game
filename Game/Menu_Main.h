#pragma once

#include "Menu.h"


class Menu_Main : public Menu
{
public:

	Menu_Main();

	virtual void drawMenu();

private:
	UI_Element_Button *pB_exit, *pB_start, *pB_options;
};

