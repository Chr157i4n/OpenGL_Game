#pragma once

#include <vector>
#include <string>
#include <unordered_map>

#include <SDL.h>

#include "MenuLayout.h"

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
	{SDLK_UP,		MenuAction::Up},
	{SDLK_DOWN,		MenuAction::Down},
	{SDLK_LEFT,		MenuAction::Left},
	{SDLK_RIGHT,	MenuAction::Right},
	{SDLK_SPACE,	MenuAction::Space},
	{SDLK_RETURN,	MenuAction::Enter},
	{SDLK_ESCAPE,	MenuAction::Esc},
};

class Menu
{
public:
	Menu();

	virtual ~Menu();

	std::vector<UI_Element*> getMenuItemList()
	{
		return menuElementList;
	}

	virtual void drawMenu();

	// The grid the menu is laid out in. Menus set it in their constructor, the
	// actual pixel positions are resolved every frame in applyLayout().
	void setLayout(MenuLayout layout)
	{
		this->layout = layout;
	}

	void addMenuElement(UI_Element* menuElement, int row = 0, int col = 0);

	MenuLayoutResult applyLayout();

	void selectNextElement();

	void selectPreviousElement();

	void leftOnSelectedMenuElement();

	void rightOnSelectedMenuElement();

	void enterSelectedMenuElement();

	void onMouseMove(float x, float y);

	int onMouseDown(float x, float y);

	int onMouseClick(float x, float y, SDL_MouseButtonEvent buttonEvent);

	int onKeyDown(SDL_Keycode key);


protected:

	int selectedItemIndex = 0;

	std::vector<UI_Element*> menuElementList;

	std::vector<MenuGridSlot> elementSlots;

	MenuLayout layout;

};
