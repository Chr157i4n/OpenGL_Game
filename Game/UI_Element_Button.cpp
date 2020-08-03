#include "UI_Element_Button.h"

#include "Game.h"

UI_Element_Button::UI_Element_Button(int x, int y, int w, int h, uint64 lifespan, std::string label, glm::vec4 color, bool isDebugInfo)
{
	this->x = x;
	this->y = y;
	this->w = w;
	this->h = h;
	this->isDebugInfo = isDebugInfo;
	this->lifespan = lifespan;
	this->color = color;
	this->label = label;
	this->backColor = glm::vec4(0.2, 0.2, 0.2, 0.4);
}

void UI_Element_Button::drawUI_Element()
{
	float _x = x;
	float _y = y;
	float _w = w;
	float _h = h;
	float outlineThickness = 0.01;

	_x /= Game::getWindowWidth();
	_y /= Game::getWindowHeight();
	_w /= Game::getWindowWidth();
	_h /= Game::getWindowHeight();

	_x = _x * 2 - 1;
	_y = _y * 2 - 1;
	_w = _w * 2;
	_h = _h * 2;

	if (isSelected)
	{
		backColor.a = 1;
	}
	else
	{
		backColor.a = 0.4;
	}

	//background
	glColor4f(backColor.r, backColor.g, backColor.b, backColor.a);
	glBegin(GL_QUADS);
	glVertex2f(_x - outlineThickness, _y - outlineThickness);
	glVertex2f(_x + _w + outlineThickness, _y - outlineThickness);
	glVertex2f(_x + _w + outlineThickness, _y + _h + outlineThickness);
	glVertex2f(_x - outlineThickness, _y + _h + outlineThickness);
	glEnd();

	UI::drawString(x, Game::getWindowHeight()-y, label, color);
}
