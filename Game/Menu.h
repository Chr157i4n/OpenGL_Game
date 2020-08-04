#pragma once

#include <vector>
#include <string>

#include <SDL.h>

#include "UI_Element.h"
#include "UI_Element_Button.h"
#include "UI_Element_Graph.h"
#include "UI_Element_Label.h"
#include "UI_Element_ProgressBar.h"
#include "UI_Element_Slider.h"
#include "Map.h"


class Menu
{
public:
	Menu();

	std::vector<UI_Element*> getMenuItemList()
	{
		return menuElementList;
	}

	void drawMenu();

	void addMenuElement(UI_Element* menuElement);

	void selectNextElement();

	void selectPreviousElement();

	void leftOnSelectedMenuElement();

	void rightOnSelectedMenuElement();

	void enterSelectedMenuElement();

	void onMouseMove(float x, float y);

	void onMouseDown(float x, float y, SDL_MouseButtonEvent buttonEvent);


protected:

	int selectedItemIndex = 0;

	std::vector<UI_Element*> menuElementList;

};

