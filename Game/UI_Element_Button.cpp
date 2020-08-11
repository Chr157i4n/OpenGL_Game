#include "UI_Element_Button.h"

#include "Game.h"

UI_Element_Button::UI_Element_Button(int x, int y, int w, int h, uint64 lifespan, std::string label, glm::vec4 foreColor, glm::vec4 backColor, bool isDebugInfo) : UI_Element(x, y, w, h, lifespan, foreColor, backColor, isDebugInfo)
{	
	this->label = label;

	labelOffsetX = w/2 - 6*label.size();		//todo: pixelsize of character are different. calculate the offset correctly
	labelOffsetY = h/2 - 10;					//todo: make it an option, how the label should be alligned

	singleAction = true;
}

void UI_Element_Button::drawUI_Element()
{
	float _x = x;
	float _y = y;
	float _w = w;
	float _h = h;
	float outlineThickness = 0;//0.01;

	_x /= Game::getWindowWidth();
	_y /= Game::getWindowHeight();
	_w /= Game::getWindowWidth();
	_h /= Game::getWindowHeight();

	_x = _x * 2 - 1;
	_y = _y * 2 - 1;
	_w = _w * 2;
	_h = _h * 2;


	float alpha=1;

	if (!isSelected)
	{
		alpha = backColor.a;
	}

	//background
	glColor4f(backColor.r, backColor.g, backColor.b, alpha);
	glBegin(GL_QUADS);
	glVertex2f(_x - outlineThickness, _y - outlineThickness);
	glVertex2f(_x + _w + outlineThickness, _y - outlineThickness);
	glVertex2f(_x + _w + outlineThickness, _y + _h + outlineThickness);
	glVertex2f(_x - outlineThickness, _y + _h + outlineThickness);
	glEnd();

	UI::drawString(x + labelOffsetX, y+labelOffsetY, label, foreColor);

}

int UI_Element_Button::onMouseClick(float mouseX, float mouseY, SDL_MouseButtonEvent* buttonEvent)
{
	return callCallBack();
}
