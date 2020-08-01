#pragma once

#include <vector>
#include <string>

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

private:
	std::vector<MenuItem*> menuItemList;

	int selectedItemIndex = 0;


};

