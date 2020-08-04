#include "UI_Element_Graph.h"
#include "Game.h"

UI_Element_Graph::UI_Element_Graph(int x, int y, int w, int h, uint64 lifespan, glm::vec4 foreColor, glm::vec4 backColor, bool isDebugInfo) : UI_Element(x, y, w, h, lifespan, foreColor, backColor, isDebugInfo)
{

}

void UI_Element_Graph::drawUI_Element()
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


	//background
	glColor4f(backColor.r, backColor.g, backColor.b, backColor.a);
	glBegin(GL_QUADS);
	glVertex2f(_x - outlineThickness, _y - outlineThickness);
	glVertex2f(_x + _w + outlineThickness, _y - outlineThickness);
	glVertex2f(_x + _w + outlineThickness, _y + _h + outlineThickness);
	glVertex2f(_x - outlineThickness, _y + _h + outlineThickness);
	glEnd();

	for (float i = 0; i < valueBuffer.size()-1; i++)
	{
		float ValueX = _x + i / valueBuffer.size()* _w;
		float ValueY = _y + _h;
		if (valueBuffer[i] < 80)
		{
			ValueY = _y + valueBuffer[i] / 80 * _h;
		}

		float ValueX2 = _x + (i+1) / valueBuffer.size() * _w;
		float ValueY2 = _y + _h;
		if (valueBuffer[(i + 1)] < 80)
		{
			ValueY2 = _y + valueBuffer[(i + 1)] / 80 * _h;
		}

		glBegin(GL_LINES);
		//glColor4f(color.r, color.g, color.b, color.a);
		if(valueBuffer[i]>=80)
			glColor4f(0, 0, 1, 1);
		else if(valueBuffer[i] > 30)
			glColor4f(1, 1, 0, 1);
		else
			glColor4f(1, 0, 0, 1);

		glVertex2f(ValueX, ValueY);
		glVertex2f(ValueX2, ValueY2);
		glEnd();
	}

}


float32 UI_Element_Graph::getValue() const
{
	return valueBuffer[valueBuffer.size()-1];
}

void UI_Element_Graph::setValue(float value)
{
	valueBuffer.push_back(value);

	if (valueBuffer.size() > 50)
	{
		valueBuffer.erase(valueBuffer.begin());
	}
}
