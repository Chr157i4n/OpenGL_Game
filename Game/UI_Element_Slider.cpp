#include "UI_Element_Slider.h"

#include "Game.h"

UI_Element_Slider::UI_Element_Slider(int x, int y, int w, int h, uint64 lifespan, std::string label, glm::vec4 foreColor, glm::vec4 backColor, bool isDebugInfo) : UI_Element(x, y, w, h, lifespan, foreColor, backColor, isDebugInfo)
{
	this->label = label;

	labelOffsetX = 0;
	labelOffsetY = h/2;
}

void UI_Element_Slider::drawUI_Element()
{
	float _x = x;
	float _y = y;
	float _w = w;
	float _h = h;
	float barThickness = 0.01;
	float SliderThickness = 0.04;
	float _value = (value - minValue) / (maxValue - minValue);

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
	glVertex2f(_x ,			_y + 0.25*_h - barThickness);
	glVertex2f(_x + _w,		_y + 0.25*_h - barThickness);
	glVertex2f(_x + _w,		_y + 0.25*_h);
	glVertex2f(_x,			_y + 0.25*_h);
	glEnd();

	//Slider
	glColor4f(foreColor.r, foreColor.g, foreColor.b, 1);
	glBegin(GL_QUADS);
	glVertex2f(_x + _value * _w - SliderThickness / 2, _y + 0.25 * _h - SliderThickness / 2);		//left bottom
	glVertex2f(_x + _value * _w + SliderThickness / 2, _y + 0.25 * _h - SliderThickness / 2);		//right bottom	
	glVertex2f(_x + _value * _w + SliderThickness / 2, _y + 0.25 * _h + SliderThickness / 2);		//right top
	glVertex2f(_x + _value * _w - SliderThickness / 2, _y + 0.25 * _h + SliderThickness / 2);		//left top
	glEnd();

	UI::drawString(x + labelOffsetX, y+labelOffsetY, label, foreColor);

}

int UI_Element_Slider::onMouseDrag(float mouseX, float mouseY, SDL_MouseButtonEvent* buttonEvent)
{
	float newValue = (mouseX - this->getX()) / this->getW();
	if (newValue < 0.05)	newValue = 0;
	if (newValue > 0.95)	newValue = 1;
	
	newValue = newValue * (maxValue - minValue) + minValue;

	this->setValue(newValue);
	return this->callCallBack();
}

int UI_Element_Slider::increase()
{
	this->setValue(this->getValue() + 1 * (maxValue - minValue) * 0.5 * Game::getDelta()/1000);
	return this->callCallBack();
}

int UI_Element_Slider::decrease()
{
	this->setValue(this->getValue() - 1 * (maxValue - minValue) * 0.5 * Game::getDelta()/1000);
	return this->callCallBack();
}
