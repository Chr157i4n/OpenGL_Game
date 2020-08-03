#pragma once

#include <vector>
#include <string>

#include <SDL.h>

#include "UI_Element.h"
#include "UI_Element_Button.h"
#include "UI_Element_Graph.h"
#include "UI_Element_Label.h"
#include "UI_Element_ProgressBar.h"

enum class MenuItemType
{
	none,
	resume,
	options,
	restart,
	exit,
};

struct MenuItem
{
	bool selected = false;
	std::string text = "";
	MenuItemType type = MenuItemType::none;
	int id = -1;
};

class Menu
{
public:
	Menu();

	std::vector<MenuItem*> getMenuItemList()
	{
		return menuItemList;
	}

	void drawMenu();

	MenuItem* getSelectedMenuItem();

	void addMenuItem(std::string text, MenuItemType menuItemType);

	void selectNextItem();

	void selectPreviousItem();

	void enterSelectedMenuItem();

	void onMouseMove(float x, float y);

	void onMouseDown(SDL_MouseButtonEvent buttonEvent);

private:
	std::vector<MenuItem*> menuItemList;

	int selectedItemIndex = 0;

	UI_Element_Button* testButton = new UI_Element_Button(10, 10, 50, 20, 0, "Test");

};

