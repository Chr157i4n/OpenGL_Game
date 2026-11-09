#include "Menu.h"

#include "Game.h"
#include "UI.h"

Menu::Menu()
{

}

Menu::~Menu()
{
	for (UI_Element* element : menuElementList)
		delete element;

	menuElementList.clear();
	elementSlots.clear();
}

void Menu::addMenuElement(UI_Element* menuElement, int row, int col)
{
	menuElement->setID((int)menuElementList.size());
	menuElementList.push_back(menuElement);

	elementSlots.push_back(MenuGridSlot{ row, col });
}

// Resolves the logical grid against the current window size, so the menus
// adapt to the resolution instead of using hard coded pixel coordinates.
MenuLayoutResult Menu::applyLayout()
{
	if (menuElementList.empty())
		return MenuLayoutResult();

	MenuLayoutResult result = layout.compute(elementSlots);

	for (size_t i = 0; i < menuElementList.size(); i++)
	{
		int x = 0, y = 0;
		layout.cellPosition(result, result.slots[i], x, y);
		menuElementList[i]->setPosition(x, y, result.cellWidth, result.cellHeight);
	}

	return result;
}

void Menu::drawMenu()
{
	applyLayout();

	GLCALL(glDisable(GL_CULL_FACE));
	GLCALL(glDisable(GL_DEPTH_TEST));

	for (size_t i = 0; i < menuElementList.size(); i++)
	{
		menuElementList[i]->drawUI_Element();
	}

	GLCALL(glEnable(GL_CULL_FACE));
	GLCALL(glEnable(GL_DEPTH_TEST));
}

void Menu::selectNextElement()
{
	int count = (int)menuElementList.size();
	if (count == 0)
		return;

	menuElementList[selectedItemIndex]->setIsSelected(false);
	selectedItemIndex = (selectedItemIndex + 1) % count;
	menuElementList[selectedItemIndex]->setIsSelected(true);
}

void Menu::selectPreviousElement()
{
	int count = (int)menuElementList.size();
	if (count == 0)
		return;

	menuElementList[selectedItemIndex]->setIsSelected(false);

	// C++ keeps the negative sign of the dividend, so (i - 1) % count can be -1.
	selectedItemIndex = ((selectedItemIndex - 1) % count + count) % count;

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
	for (int i = (int)menuElementList.size() - 1; i >= 0; i--)
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

int Menu::onMouseDown(float x, float y)
{
	for (int i = (int)menuElementList.size() - 1; i >= 0; i--)
	{
		if (menuElementList[i]->isMouseOver(x, y))
		{
			return menuElementList[i]->onMouseDrag(x, y);
		}
	}

	return MENU_OK;
}

int Menu::onMouseClick(float x, float y, SDL_MouseButtonEvent buttonEvent)
{
	if (buttonEvent.button == SDL_BUTTON_LEFT)
	{
		for (int i = (int)menuElementList.size() - 1; i >= 0; i--)
		{
			if (menuElementList[i]->isMouseOver(x, y))
			{
				return menuElementList[i]->onMouseClick(x, y, &buttonEvent);
			}
		}
	}

	return MENU_OK;
}

int Menu::onKeyDown(SDL_Keycode key)
{
	if (menuElementList.empty())
		return MENU_OK;

	MenuAction action = MenuAction::None;

	int returnvalue = menuElementList[selectedItemIndex]->onKeyDown(key);
	if (returnvalue != 0) return returnvalue;

	auto it = menu_keybindings.find(key);
	if (it != menu_keybindings.end()) {
		action = it->second;
	}
	if (action == MenuAction::None) return MENU_OK;

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
		case MenuAction::Left:
			this->leftOnSelectedMenuElement();
			break;
		case MenuAction::Right:
			this->rightOnSelectedMenuElement();
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

	return MENU_OK;
}
