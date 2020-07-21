#include "UI_Element_ProgressBar.h"
#include "UI.h"

UI_Element_ProgressBar::UI_Element_ProgressBar(int x, int y, int w, int h, float value, uint64 lifespan, bool isDebugInfo) : UI_Element()
{
	this->value = value;
	this->x = x;
	this->y = y;
	this->w = w;
	this->h = h;
	this->isDebugInfo = isDebugInfo;
	this->lifespan = lifespan;
}

void UI_Element_ProgressBar::drawUI_Element()
{
	glColor3f(0, 0, 1);
	float _x = x;
	float _y = y;
	float _w = w;
	float _h = h;
	float _value = value / 100;
	float outlineThickness = 0.01;

	_x /= UI::getWidth();
	_y /= UI::getHeight();
	_w /= UI::getWidth();
	_h /= UI::getHeight();

	_x = _x*2-1;
	_y = _y*2-1;
	//_w = _w*2-1;
	//_h = _h*2-1;

	//Bar
	glBegin(GL_QUADS);
	glVertex2f(_x				, _y);
	glVertex2f(_x + _value*_w 	, _y);
	glVertex2f(_x + _value*_w	, _y + _h);
	glVertex2f(_x				, _y + _h);
	glEnd();

	//top - outline
	glColor3f(0.2, 0.2, 0.2);
	glBegin(GL_QUADS);
	glVertex2f(_x - outlineThickness		, _y - outlineThickness);
	glVertex2f(_x + _w + outlineThickness	, _y - outlineThickness);
	glVertex2f(_x + _w + outlineThickness	, _y + _h + outlineThickness);
	glVertex2f(_x - outlineThickness		, _y + _h + outlineThickness);
	glEnd();
}


float UI_Element_ProgressBar::getValue() const
{
	return value;
}

void UI_Element_ProgressBar::setValue(float value)
{
	this->value = value;
}

