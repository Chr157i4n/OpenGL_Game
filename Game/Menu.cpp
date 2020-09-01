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
	GLCALL(glDisable(GL_CULL_FACE));
	GLCALL(glDisable(GL_DEPTH_TEST));
	//int x = Game::getWindowWidth() / 2 - 50;
	//int y = Game::getWindowHeight() / 2 - menuElementList.size() * 40 / 2;

	for (int i = 0; i < menuElementList.size(); i++)
	//for (int i = menuElementList.size() - 1; i >= 0; i--)
	{
		menuElementList[i]->drawUI_Element();
	}

	GLCALL(glEnable(GL_CULL_FACE));
	GLCALL(glEnable(GL_DEPTH_TEST));
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
	for (int i = menuElementList.size() - 1; i >= 0; i--)
	{

		if (menuElementList[i]->isMouseOver(x, y))
		{
			for (UI_Element* element : menuElementList)
			{
				element->setIsSelected(false);
			}

			menuElementList[i]->setIsSelected(true);
			selectedItemIndex = menuElementList[i]->getID();
			return;
		}
	}
}

int Menu::onMouseDown(float x, float y, SDL_MouseButtonEvent buttonEvent)
{
	if (buttonEvent.button == SDL_BUTTON_LEFT)
	{
		for (int i = menuElementList.size() - 1; i >= 0; i--)
		{
			if (menuElementList[i]->isMouseOver(x, y))
			{
				return menuElementList[i]->onMouseDrag(x, y, &buttonEvent);
			}
		}
	}
}

int Menu::onMouseClick(float x, float y, SDL_MouseButtonEvent buttonEvent)
{
	if (buttonEvent.button == SDL_BUTTON_LEFT)
	{
		for (int i = menuElementList.size() - 1; i >= 0; i--)
		{
			if (menuElementList[i]->isMouseOver(x, y))
			{
				return menuElementList[i]->onMouseClick(x, y, &buttonEvent);
			}
		}
	}
}

int Menu::onKeyDown(SDL_Keycode key)
{
	MenuAction action = MenuAction::None;

	int returnvalue = menuElementList[selectedItemIndex]->onKeyDown(key);
	if (returnvalue != 0) return returnvalue;


	auto it = menu_keybindings.find(key);
	if (it != menu_keybindings.end()) {
		action = it->second;
	}
	if (action == MenuAction::None) { Logger::log("Keybinding not found!"); return -1; }

	switch (action)
	{
		case MenuAction::Up:
			this->selectPreviousElement();
			AudioManager::play2D("audio/select.wav");
			break;
		case MenuAction::Down:
			this->selectNextElement();
			AudioManager::play2D("audio/select.wav");
			break;
		case MenuAction::Enter:
			this->enterSelectedMenuElement();
			AudioManager::play2D("audio/select.wav");
			break;
		case MenuAction::Space:
			this->enterSelectedMenuElement();
			AudioManager::play2D("audio/select.wav");
			break;
	}

	return 0;
}
