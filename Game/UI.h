#pragma once

#include <iomanip>
#include <sstream>
#include <SDL.h>
#include <cstring>
#include <string>

#include "Font.h"
#include "Shader.h"
#include "Object.h"

struct UI_Element
{
	int posX, posY;
	std::string text;
};

static class UI
{
public:

	static void init(SDL_Window* newWindow);

	static void drawFPS(int fps);

	static void drawPos(Object* object);

	static void drawString(float x, float y, std::string text);

	static int getHeight();

	static int getWidth();

	static void drawUI();

	static void addElement(UI_Element* newElement);

private:
	static SDL_Window* window;
	static Shader* fontShader;
	static Font* font;

	static int w, h;

	static std::vector<UI_Element*> ui_elements;

};

