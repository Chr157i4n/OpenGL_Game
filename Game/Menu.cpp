#include "Menu.h"

#include "Game.h"
#include "UI.h"

Menu::Menu()
{

}

void Menu::addMenuElement(UI_Element* menuElement)
{
	menuElement->setID(menuElementList.size());
	menuElementList.push_back(menuElement);
}

void Menu::drawMenu()
{
	//int x = Game::getWindowWidth() / 2 - 50;
	//int y = Game::getWindowHeight() / 2 - menuElementList.size() * 40 / 2;


	for (UI_Element* element : menuElementList)
	{
		element->drawUI_Element();
	}
}

void Menu::selectNextElement()
{
	menuElementList[selectedItemIndex]->setIsSelected(false);
	selectedItemIndex = (selectedItemIndex + 1) % menuElementList.size();
	menuElementList[selectedItemIndex]->setIsSelected(true);
}

void Menu::selectPreviousElement()
{
	menuElementList[selectedItemIndex]->setIsSelected(false);
	selectedItemIndex = (selectedItemIndex - 1) % menuElementList.size();
	menuElementList[selectedItemIndex]->setIsSelected(true);
}

void Menu::leftOnSelectedMenuElement()
{
	menuElementList[selectedItemIndex]->decrease();
}

void Menu::rightOnSelectedMenuElement()
{
	menuElementList[selectedItemIndex]->increase();
}

void Menu::enterSelectedMenuElement()
{
	menuElementList[selectedItemIndex]->callCallBack();
}



void Menu::onMouseMove(float x, float y)
{
	//Logger::log("Mouse Position: " + std::to_string(x) + " " + std::to_string(y));
	for (UI_Element* element : menuElementList)
	{

		if (element->isMouseOver(x, y))
		{
			for (UI_Element* element : menuElementList)
			{
				element->setIsSelected(false);
			}

			element->setIsSelected(true);
			selectedItemIndex = element->getID();
		}
	}
}

void Menu::onMouseDown(float x, float y, SDL_MouseButtonEvent buttonEvent)
{
	if (buttonEvent.button == SDL_BUTTON_LEFT)
	{
		for (UI_Element* element : menuElementList)
		{
			if (element->isMouseOver(x, y))
			{
				element->action(x, y);
				element->callCallBack();
			}
		}
	}
}