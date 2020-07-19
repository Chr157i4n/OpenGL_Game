#pragma once

#include <iomanip>
#include <sstream>
#include <SDL.h>
#include <cstring>
#include <string>

#include "Font.h"
#include "Shader.h"
#include "Object.h"
#include "Player.h"

struct UI_Element
{
	int posX, posY;
	std::string text;
	glm::vec4 color;
	bool debugInfo;
};

enum class MenuItemType
{
	none,
	resume,
	options,
	restart,
	exit,
};

struct MenuItem
{
	bool selected = false;
	std::string text = "";
	MenuItemType type= MenuItemType::none;
};

static class UI
{
public:

	static void init(SDL_Window* newWindow);

	static void drawFPS(int fps);

	static void drawPos(std::shared_ptr<Object> object);

	static void drawRot(std::shared_ptr<Player> object);

	static void drawString(float x, float y, std::string text, glm::vec4 color);

	static void drawVec3(glm::vec3 vector, int precision, std::string text, float x, float y);

	static int getHeight();

	static int getWidth();

	static void drawUI();

	static void addElement(UI_Element* newElement);

	static void drawMenu();

	static void drawPause();

	static std::vector<MenuItem*> getMenuItemList()
	{
		return menuItemList;
	}

	static MenuItem* getSelectedMenuItem();



private:
	static SDL_Window* window;
	static Shader* fontShader;
	static Font* font;

	static int fontColorUniformIndex;

	static int width, height;

	static std::vector<UI_Element*> ui_elements;

	static std::vector<MenuItem*> menuItemList;

};

