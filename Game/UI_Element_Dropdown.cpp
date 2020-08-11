#include "UI_Element_Dropdown.h"

#include "Game.h"

UI_Element_Dropdown::UI_Element_Dropdown(int x, int y, int w, int h, uint64 lifespan, glm::vec4 foreColor, glm::vec4 backColor, bool isDebugInfo) : UI_Element(x, y, w, h, lifespan, foreColor, backColor, isDebugInfo)
{

}

void UI_Element_Dropdown::drawUI_Element()
{
	float outlineThickness = 2;//0.01;
	//backColor.a = 1;

	//box of selected item
	glColor4f(backColor.r, backColor.g, backColor.b, backColor.a);
	glBegin(GL_QUADS);
	glVertex2f(UI::to_x_gl(x),		UI::to_y_gl(y));
	glVertex2f(UI::to_x_gl(x + w),	UI::to_y_gl(y));
	glVertex2f(UI::to_x_gl(x + w),	UI::to_y_gl(y + h));
	glVertex2f(UI::to_x_gl(x),		UI::to_y_gl(y + h));
	glEnd();

	if (selectedItem)
	{
		UI::drawString(x + selectedItem->labelOffsetX, y + selectedItem->labelOffsetY, selectedItem->label, foreColor);
	}
	else
	{
		UI::drawString(x + w / 2 - 5 * this->name.size(), y + h / 2 - 10, this->name, foreColor);
	}


	if(dropdown_open)
	{

		//outline of all items
		glColor4f(backColor.r * 0.6, backColor.g * 0.6, backColor.b * 0.6, backColor.a);
		glBegin(GL_QUADS);
		glVertex2f(UI::to_x_gl(x + w), UI::to_y_gl(y));
		glVertex2f(UI::to_x_gl(x + 1.0 / 4.0 * w), UI::to_y_gl(y));
		glVertex2f(UI::to_x_gl(x + 1.0 / 4.0 * w), UI::to_y_gl(y - h * items.size()));
		glVertex2f(UI::to_x_gl(x + w), UI::to_y_gl(y - h * items.size()));
		glEnd();

		for (int i = 0; i < items.size(); i++)
		{

			float alpha = 1;
			if (!items[i]->isSelected)
			{
				alpha = backColor.a;
			}

			//outline of single item
			glColor4f(backColor.r * 0.8, backColor.g * 0.8, backColor.b * 0.8, alpha);
			glBegin(GL_QUADS);
			glVertex2f(UI::to_x_gl(x + w- outlineThickness),				UI::to_y_gl(y - (i)*h- outlineThickness));
			glVertex2f(UI::to_x_gl(x + 1.0 / 4.0 * w+ outlineThickness),	UI::to_y_gl(y - (i)*h- outlineThickness));
			glVertex2f(UI::to_x_gl(x + 1.0 / 4.0 * w+ outlineThickness),	UI::to_y_gl(y - (i + 1) * h+ outlineThickness));
			glVertex2f(UI::to_x_gl(x + w- outlineThickness),				UI::to_y_gl(y - (i + 1) * h+ outlineThickness));
			glEnd();

			//test of single item
			UI::drawString(x + items[i]->labelOffsetX, y - (i + 1) * h + items[i]->labelOffsetY, items[i]->label, foreColor);

		}
	}
}

void UI_Element_Dropdown::addItem(UI_Element_Dropdown_Item* item)
{
	item->ID = items.size();
	item->labelOffsetX = w / 2 - 4 * item->label.size();		//todo: pixelsize of character are different. calculate the offset correctly
	item->labelOffsetY = h / 2 - 10;

	items.push_back(item);
}

bool UI_Element_Dropdown::isMouseOver(float mouseX, float mouseY)
{
	if (dropdown_open)
	{
		if (
			(mouseX > this->getX()) &&
			(mouseX < this->getX() + this->getW()) &&
			(Game::getWindowHeight() - mouseY > this->getY() - this->getH() * this->items.size()) &&
			(Game::getWindowHeight() - mouseY < this->getY() + this->getH())
			)
		{
			return true;
		}
	}
	else
	{
		if (
			(mouseX > this->getX()) &&
			(mouseX < this->getX() + this->getW()) &&
			(Game::getWindowHeight() - mouseY > this->getY()) &&
			(Game::getWindowHeight() - mouseY < this->getY() + this->getH())
			)
		{
			return true;
		}

	}

	return false;
}

int UI_Element_Dropdown::onMouseClick(float mouseX, float mouseY, SDL_MouseButtonEvent* buttonEvent)
{
	if (mouseX > this->getX() &&
		mouseX < this->getX() + this->getW() &&
		Game::getWindowHeight() - mouseY > this->getY() &&
		Game::getWindowHeight() - mouseY < this->getY() + this->getH())
	{
		toggleOpen();
	}
	else
	{
		for (int i = 0; i < items.size(); i++)
		{
			if ((mouseX > this->getX()) &&
				(mouseX < this->getX() + this->getW()) && 
				((Game::getWindowHeight() - mouseY) < (this->getY() - this->getH() * i)) &&  
				((Game::getWindowHeight() - mouseY) > (this->getY() - this->getH() * (i+1))))
			{
				if(selectedItem)
					selectedItem->isSelected = false;
					selectedItem = items[i];
					selectedItem->isSelected = true;
					return selectedItem->callback();
			}
		}
	}

	return 0;
}
