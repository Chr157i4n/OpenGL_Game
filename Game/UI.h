#pragma once
#include "defines.h"

#include <iomanip>
#include <sstream>
#include <SDL.h>
#include <cstring>
#include <string>

#include "Font.h"
#include "Object.h"
#include "UI_Element.h"
#include "Shader.h"
#include "Player.h"
#include "Menu.h"

static class UI
{
public:

	static void init();

	static void drawFPS();

	static void drawPos(std::shared_ptr<Object> object);

	static void drawRot(std::shared_ptr<Player> object);

	static void drawString(float x, float y, std::string text, glm::vec4 color);

	static void drawVec3(glm::vec3 vector, int precision, std::string text, float x, float y);

	static void drawUI();

	static void addElement(UI_Element* newElement);

	static void drawPause();

	static void checkLifeSpan();

	static void clearUI_Elements();

	static void clearMessages();


private:
	static Shader* fontShader;
	static Font* font;

	static int fontColorUniformIndex;

	static std::vector<UI_Element*> ui_elements;

};

