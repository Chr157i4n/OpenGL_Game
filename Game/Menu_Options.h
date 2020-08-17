#pragma once

#include "Menu.h"

class Menu_Options : public Menu
{
public:

	Menu_Options();

	void toggleVsync();

	void toggleFullscreen();

	void setVolume();

	void toggleShadows();

	void changeShadowMapResolution();

	void changeEnvMapResolution();

private:
	UI_Element_Button *pB_vsync, *pB_shadow, *pB_back, *pB_fullscreen;
	UI_Element_Slider *sL_shadowres, * sL_envres;
	UI_Element_Slider* sL_volume_master, * sL_volume_music, * sL_volume_effect, * sL_volume_ambient, * sL_volume_voice;
	UI_Element_Dropdown *dD_Resolution;
};

