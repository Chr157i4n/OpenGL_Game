#pragma once

#include "Menu.h"

class Menu_Options : public Menu
{
public:

	Menu_Options();

	void toggleVsync();

	void setVolume();

	void toggleShadows();

private:
	UI_Element_Button *pB_resume, *pB_vsync, *pB_shadow, *pB_back, *pB_test;
	UI_Element_Slider *sL_volume;
	UI_Element_Dropdown *dD_Resolution;
};

