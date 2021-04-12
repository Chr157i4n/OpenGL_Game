#include "UI_Element_TextEdit.h"

#include "UI.h"
#include "Game.h"

UI_Element_TextEdit::UI_Element_TextEdit(int x, int y, int w, int h, std::string label, uint64 lifespan, int fontSize, glm::vec4 foreColor, glm::vec4 backColor, bool isDebugInfo) : UI_Element(x, y, w, h, lifespan, foreColor, backColor, isDebugInfo)
{
	text = label;
	labelOffsetX = 15;							//todo: pixelsize of character are different. calculate the offset correctly
	labelOffsetY = h / 2 - 10;					//todo: make it an option, how the label should be alligned
}

void UI_Element_TextEdit::drawUI_Element()
{
	float _x = x;
	float _y = y;
	float _w = w;
	float _h = h;
	float outlineThickness = 0.02;

	_x /= Game::getWindowWidth();
	_y /= Game::getWindowHeight();
	_w /= Game::getWindowWidth();
	_h /= Game::getWindowHeight();

	_x = _x * 2 - 1;
	_y = _y * 2 - 1;
	_w = _w * 2;
	_h = _h * 2;


	float alpha = 1;

	if (!isSelected)
	{
		alpha = backColor.a;
	}

	//background
	glColor4f(backColor.r, backColor.g, backColor.b, alpha);
	glBegin(GL_QUADS);
	glVertex2f(_x,		_y);
	glVertex2f(_x + _w, _y);
	glVertex2f(_x + _w, _y + _h);
	glVertex2f(_x,		_y + _h);
	glEnd();

	//box
	glColor4f(backColor.r * 0.8, backColor.g * 0.8, backColor.b * 0.8, alpha*1.4);
	glBegin(GL_QUADS);
	glVertex2f(_x + outlineThickness,		_y + outlineThickness);
	glVertex2f(_x + _w - outlineThickness,	_y + outlineThickness);
	glVertex2f(_x + _w - outlineThickness,	_y + _h - outlineThickness);
	glVertex2f(_x + outlineThickness,		_y + _h - outlineThickness);
	glEnd();



	UI::drawString(x + labelOffsetX, y + labelOffsetY, text, foreColor);
}

int UI_Element_TextEdit::onKeyDown(SDL_Keycode key)
{
	if (key == SDLK_BACKSPACE)
	{
		text = text.substr(0, text.size()-1);
		callCallBack();
		return 1;
	}

	std::string input = SDL_GetKeyName(key);


	std::vector<std::string> toDeletes;
	toDeletes.push_back("Keypad ");

	for (std::string toDelete : toDeletes)
	{
		size_t pos = input.find(toDelete);
		if (pos != std::string::npos)
		{
			// If found then erase it from string
			input.erase(pos, toDelete.length());
		}
	}

	if(text.length() < maxLength)
		if (input.length() == 1)
		{
			if (std::regex_match(input, *regExp))
			{
				if (Game::isKeyPressed(SDLK_LSHIFT))
				{
					std::transform(input.begin(), input.end(), input.begin(), [](unsigned char c) { return std::toupper(c); });
				}
				else
				{
					std::transform(input.begin(), input.end(), input.begin(), [](unsigned char c) { return std::tolower(c); });
				}

				text += input;
			}
						
		}

	callCallBack();
	return 1;
}
