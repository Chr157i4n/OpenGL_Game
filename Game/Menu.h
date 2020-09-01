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
#include "UI_Element_Boxed_Label.h"
#include "UI_Element_Dropdown.h"
#include "UI_Element_TextEdit.h"

#include "Map.h"

enum class MenuAction {
	None,
	Up,
	Down,
	Left,
	Right,
	Space,
	Enter,
	Esc,
};

std::unordered_map<SDL_Keycode, MenuAction> const menu_keybindings =
{
	{SDLK_w,		MenuAction::Up},
	{SDLK_s,		MenuAction::Down},
	{SDLK_a,		MenuAction::Left},
	{SDLK_d,		MenuAction::Right},
	{SDLK_SPACE,	MenuAction::Space},
	{SDLK_RETURN,	MenuAction::Enter},
	{SDLK_ESCAPE,	MenuAction::Esc},
};

class Menu
{
public:
	Menu();

	std::vector<UI_Element*> getMenuItemList()
	{
		return menuElementList;
	}

	virtual void drawMenu();

	void addMenuElement(UI_Element* menuElement);

	void selectNextElement();

	void selectPreviousElement();

	void leftOnSelectedMenuElement();

	void rightOnSelectedMenuElement();

	void enterSelectedMenuElement();

	void onMouseMove(float x, float y);

	int onMouseDown(float x, float y, SDL_MouseButtonEvent buttonEvent);

	int onMouseClick(float x, float y, SDL_MouseButtonEvent buttonEvent);

	int onKeyDown(SDL_Keycode key);


protected:

	int selectedItemIndex = 0;

	std::vector<UI_Element*> menuElementList;

};

