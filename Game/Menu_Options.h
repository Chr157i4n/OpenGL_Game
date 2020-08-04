#pragma once

#include "Menu.h"

class Menu_Options : public Menu
{
public:

	Menu_Options();

	void toggleVsync();

	void setVolume();

private:
	UI_Element_Button* pB_resume, * pB_vsync, * pB_options, * pB_back;
	UI_Element_Slider* sL_volume;
};

