#include "UI_Element_Dropdown.h"

#include <algorithm>

#include "Game.h"

UI_Element_Dropdown::UI_Element_Dropdown(int x, int y, int w, int h, uint64 lifespan, glm::vec4 foreColor, glm::vec4 backColor, bool isDebugInfo) : UI_Element(x, y, w, h, lifespan, foreColor, backColor, isDebugInfo)
{
	updateOpenDirection();
}

// Returns the window y coordinate of the border between the box and the item
// list, and the direction the list grows in (+1 upwards, -1 downwards).
void UI_Element_Dropdown::updateOpenDirection()
{
	int spaceBelow = y;
	int spaceAbove = Game::getWindowHeight() - (y + h);

	dir = (spaceBelow < spaceAbove) ? 1 : -1;
}

// Window y of the i-th item border: border 0 is the box edge, border n the far end of the list.
int itemBorder(int i, int y, int h, int dir)
{
	return y + dir * i * h;
}

void UI_Element_Dropdown::drawUI_Element()
{
	updateOpenDirection();

	float outlineThickness = 2;

	int listEnd = itemBorder((int)items.size(), y, h, dir);
	int listLow = std::min(y, listEnd);
	int listHigh = std::max(y, listEnd);

	//box of the dropdown itself
	float boxAlpha = isSelected ? 1.0f : backColor.a;
	glColor4f(backColor.r, backColor.g, backColor.b, boxAlpha);
	glBegin(GL_QUADS);
	glVertex2f(UI::to_x_gl(x),			UI::to_y_gl(y));
	glVertex2f(UI::to_x_gl(x + w),		UI::to_y_gl(y));
	glVertex2f(UI::to_x_gl(x + w),		UI::to_y_gl(y + h));
	glVertex2f(UI::to_x_gl(x),			UI::to_y_gl(y + h));
	glEnd();

	if (selectedItem)
	{
		UI::drawString(x + w / 2 - 4 * (int)selectedItem->label.size(), y + h / 2 - 10, selectedItem->label, foreColor);
	}
	else
	{
		UI::drawString(x + w / 2 - 5 * (int)name.size(), y + h / 2 - 10, name, foreColor);
	}

	if (dropdown_open)
	{
		//outline of all items
		glColor4f(backColor.r * 0.6, backColor.g * 0.6, backColor.b * 0.6, backColor.a);
		glBegin(GL_QUADS);
		glVertex2f(UI::to_x_gl(x + w),		UI::to_y_gl(listLow));
		glVertex2f(UI::to_x_gl(x + w / 4),	UI::to_y_gl(listLow));
		glVertex2f(UI::to_x_gl(x + w / 4),	UI::to_y_gl(listHigh));
		glVertex2f(UI::to_x_gl(x + w),		UI::to_y_gl(listHigh));
		glEnd();

		for (int i = 0; i < (int)items.size(); i++)
		{
			float alpha = items[i]->isSelected ? 1 : backColor.a;

			float a = itemBorder(i, y, h, dir) + dir * outlineThickness;
			float b = itemBorder(i + 1, y, h, dir) - dir * outlineThickness;

			//outline of single item
			glColor4f(backColor.r * 0.8, backColor.g * 0.8, backColor.b * 0.8, alpha);
			glBegin(GL_QUADS);
			glVertex2f(UI::to_x_gl(x + w),		UI::to_y_gl(a));
			glVertex2f(UI::to_x_gl(x + w / 4),	UI::to_y_gl(a));
			glVertex2f(UI::to_x_gl(x + w / 4),	UI::to_y_gl(b));
			glVertex2f(UI::to_x_gl(x + w),		UI::to_y_gl(b));
			glEnd();

			//label of single item
			items[i]->labelOffsetX = w / 2 - 4 * (int)items[i]->label.size();
			items[i]->labelOffsetY = h / 2 - 10;

			UI::drawString(x + items[i]->labelOffsetX, itemBorder(i, y, h, dir) + items[i]->labelOffsetY, items[i]->label, foreColor);
		}
	}
}

void UI_Element_Dropdown::addItem(UI_Element_Dropdown_Item* item)
{
	item->ID = (int)items.size();
	item->labelOffsetX = w / 2 - 4 * (int)item->label.size();		//todo: pixelsize of character are different. calculate the offset correctly
	item->labelOffsetY = h / 2 - 10;

	items.push_back(item);
}

void UI_Element_Dropdown::selectItem(UI_Element_Dropdown_Item* item)
{
	if (selectedItem)
		selectedItem->isSelected = false;

	selectedItem = item;
	if (selectedItem)
		selectedItem->isSelected = true;
}

bool UI_Element_Dropdown::isMouseOver(float mouseX, float mouseY)
{
	updateOpenDirection();

	if (mouseX <= this->getX() || mouseX >= this->getX() + this->getW())
		return false;

	if (!dropdown_open)
	{
		return (Game::getWindowHeight() - mouseY) > this->getY() &&
			   (Game::getWindowHeight() - mouseY) < this->getY() + this->getH();
	}

	int listEnd = itemBorder((int)items.size(), this->getY(), this->getH(), dir);
	int listLow = std::min(this->getY(), listEnd);
	int listHigh = std::max(this->getY() + this->getH(), listEnd);

	return (Game::getWindowHeight() - mouseY) > listLow &&
		   (Game::getWindowHeight() - mouseY) < listHigh;
}

int UI_Element_Dropdown::onMouseClick(float mouseX, float mouseY, SDL_MouseButtonEvent* buttonEvent)
{
	updateOpenDirection();

	int windowY = Game::getWindowHeight() - (int)mouseY;

	if (mouseX > this->getX() &&
		mouseX < this->getX() + this->getW() &&
		windowY > this->getY() &&
		windowY < this->getY() + this->getH())
	{
		toggleOpen();
		return MENU_CONSUMED;
	}

	if (!dropdown_open)
		return MENU_OK;

	for (int i = 0; i < (int)items.size(); i++)
	{
		float a = itemBorder(i, this->getY(), this->getH(), dir);
		float b = itemBorder(i + 1, this->getY(), this->getH(), dir);

		float low = std::min(a, b);
		float high = std::max(a, b);

		if (mouseX > this->getX() && mouseX < this->getX() + this->getW() && windowY > low && windowY < high)
		{
			selectItem(items[i]);

			int result = MENU_OK;
			if (items[i]->callback)
				result = items[i]->callback();

			close();
			return result;
		}
	}

	return MENU_OK;
}

int UI_Element_Dropdown::navigateMenu(int direction)
{
	if (items.empty())
		return 0;

	if (!dropdown_open)
	{
		open();
		selectItem(items[0]);
		return 1;
	}

	int index = getSelectedIndex();
	if (index < 0) index = 0;

	index = (index + direction + (int)items.size()) % (int)items.size();
	selectItem(items[index]);

	return 1;
}

int UI_Element_Dropdown::increase()
{
	return navigateMenu(1);
}

int UI_Element_Dropdown::decrease()
{
	return navigateMenu(-1);
}

int UI_Element_Dropdown::callCallBack(SDL_MouseButtonEvent* buttonEvent)
{
	if (items.empty())
		return MENU_OK;

	if (!dropdown_open)
	{
		open();
		selectItem(items[0]);
		return MENU_OK;
	}

	if (!selectedItem)
		selectItem(items[0]);

	int result = MENU_OK;
	if (selectedItem->callback)
		result = selectedItem->callback();

	close();

	return result;
}
