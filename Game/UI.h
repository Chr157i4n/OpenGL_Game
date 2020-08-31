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
#include "Game.h"

static class UI
{
public:

	static void init();

	static void updateFPS();

	static void updatePos(std::shared_ptr<Object> object);

	static void updateRot(std::shared_ptr<Player> object);

	static void drawString(float x, float y, std::string text, glm::vec4 color);// , glm::vec3 scale = glm::vec3(1, 1, 1));

	static void drawVec3(glm::vec3 vector, int precision, std::string text, float x, float y);

	static void drawUI();

	static void addElement(UI_Element* newElement);

	static void drawPause();

	static void checkLifeSpan();

	static void clearUI_Elements();

	static void clearMessages();

	static float to_x_gl(int x_px);

	static float to_y_gl(int y_px);

	static int to_x_px(float x_gl);

	static int to_y_px(float y_gl);


	static float to_w_gl(int w_px);

	static float to_h_gl(int h_px);

	static int to_w_px(float w_gl);

	static int to_h_px(float h_gl);


private:
	static Shader* fontShader;
	static Font* font;

	static int fontColorUniformIndex;

	static std::vector<UI_Element*> ui_elements;

	static UI_Element_Label* fpsLabel;
	static UI_Element_Label* posLabel[4];
	static UI_Element_Label* rotLabel[4];

	static std::list<float> fpsBuffer;
};

