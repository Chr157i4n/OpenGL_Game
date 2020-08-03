#include "Menu.h"

#include "Game.h"
#include "UI.h"

Menu::Menu()
{
	MenuItem* newItem;
	newItem = new MenuItem{ true,"Zurueck",MenuItemType::resume };
	menuItemList.push_back(newItem);
	newItem = new MenuItem{ false,"Neustart",MenuItemType::restart };
	menuItemList.push_back(newItem);
	newItem = new MenuItem{ false,"Einstellungen",MenuItemType::options };
	menuItemList.push_back(newItem);
	newItem = new MenuItem{ false,"Beenden",MenuItemType::exit };
	menuItemList.push_back(newItem);
}

MenuItem* Menu::getSelectedMenuItem()
{
	for (MenuItem* menuItem : menuItemList)
	{
		if (menuItem->selected)
		{
			return menuItem;
		}
	}
	return menuItemList[0];
}

void Menu::addMenuItem(std::string text, MenuItemType menuItemType)
{
	MenuItem* newItem;
	int index = menuItemList.size();
	newItem = new MenuItem{ false,"Beenden",MenuItemType::exit, index };

	if (menuItemList.size() == 0)
	{
		newItem->selected = true;
	}

	menuItemList.push_back(newItem);
}

void Menu::drawMenu()
{
	int x = Game::getWindowWidth() / 2 - 50;
	int y = Game::getWindowHeight() / 2 - menuItemList.size() * 40 / 2;


	for (MenuItem* menuItem : menuItemList)
	{
		if (menuItem->selected)
		{
			UI::drawString(x, y, menuItem->text, glm::vec4(0, 0, 1, 1));
		}
		else
		{
			UI::drawString(x, y, menuItem->text, glm::vec4(1, 1, 1, 1));
		}

		y += 40;
	}


	testButton->drawUI_Element();

	//delete testButton;
}


void Menu::selectNextItem()
{
	menuItemList[selectedItemIndex]->selected = false;
	selectedItemIndex = (selectedItemIndex + 1) % menuItemList.size();
	menuItemList[selectedItemIndex]->selected = true;
}

void Menu::selectPreviousItem()
{
	menuItemList[selectedItemIndex]->selected = false;
	selectedItemIndex = (selectedItemIndex - 1) % menuItemList.size();
	menuItemList[selectedItemIndex]->selected = true;
}

void Menu::enterSelectedMenuItem()
{
	MenuItem* selectedMenuItem = getSelectedMenuItem();

	if (selectedMenuItem->type == MenuItemType::resume)
	{
		Game::toggleMenu();
	}
	if (selectedMenuItem->type == MenuItemType::exit)
	{
		SDL_DestroyWindow(Game::window);
		exit(0);
	}
	if (selectedMenuItem->type == MenuItemType::restart)
	{
		Map::restart();
	}
}

void Menu::onMouseMove(float x, float y)
{
	//Logger::log("Mouse Position: " + std::to_string(x) + " " + std::to_string(y));

	testButton->setIsSelected(false);


	if (x > testButton->getX() && x < testButton->getX() + testButton->getW() && Game::getWindowHeight() - y > testButton->getY() && Game::getWindowHeight() - y < testButton->getY() + testButton->getH())
	{
		testButton->setIsSelected(true);
	}
}

void Menu::onMouseDown(SDL_MouseButtonEvent buttonEvent)
{
	if (buttonEvent.button == SDL_BUTTON_LEFT)
	{
		if (testButton->getIsSelected())
		{
			Map::restart();
		}
	}
}