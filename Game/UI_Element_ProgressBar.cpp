#include "UI_Element_ProgressBar.h"
#include "Game.h"

UI_Element_ProgressBar::UI_Element_ProgressBar(int x, int y, int w, int h, float value, uint64 lifespan, glm::vec4 foreColor, glm::vec4 backColor, bool isDebugInfo) : UI_Element(x, y, w, h, lifespan, foreColor, backColor, isDebugInfo)
{
	this->value = value;

}

void UI_Element_ProgressBar::drawUI_Element()
{
	float _x = x;
	float _y = y;
	float _w = w;
	float _h = h;
	float _value = value / 100;
	float outlineThickness = 0.01;

	_x /= Game::getWindowWidth();
	_y /= Game::getWindowHeight();
	_w /= Game::getWindowWidth();
	_h /= Game::getWindowHeight();

	_x = _x*2-1;
	_y = _y*2-1;
	_w = _w*2;
	_h = _h*2;


	//outline
	glColor4f(backColor.r, backColor.g, backColor.b, backColor.a);
	glBegin(GL_QUADS);
	glVertex2f(_x - outlineThickness, _y - outlineThickness);
	glVertex2f(_x + _w + outlineThickness, _y - outlineThickness);
	glVertex2f(_x + _w + outlineThickness, _y + _h + outlineThickness);
	glVertex2f(_x - outlineThickness, _y + _h + outlineThickness);
	glEnd();

	//bar
	glBegin(GL_QUADS);
	glColor4f(foreColor.r, foreColor.g, foreColor.b, foreColor.a);
	glVertex2f(_x				, _y);
	glVertex2f(_x + _value*_w 	, _y);
	glVertex2f(_x + _value*_w	, _y + _h);
	glVertex2f(_x				, _y + _h);
	glEnd();

}


float UI_Element_ProgressBar::getValue() const
{
	return value;
}

void UI_Element_ProgressBar::setValue(float value)
{
	if (value>=0 && value<=100)
	{
		this->value = value;
	} 
	else if (value > 100)
	{
		this->value = 100;
	}
	else if (value < 0)
	{
		this->value = 0;
	}
}

