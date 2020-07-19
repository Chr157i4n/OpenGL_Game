#include "UI.h"


SDL_Window* UI::window;
Shader* UI::fontShader;
Font* UI::font;
int UI::fontColorUniformIndex;
int UI::w, UI::h;
std::vector<UI_Element*> UI::ui_elements;

std::vector<MenuItem*> UI::menuItemList;

void UI::init(SDL_Window* newWindow)
{
	window = newWindow;

	font = new Font();
	font->initFont("fonts/OpenSans-Regular.ttf");

	fontShader = new Shader("shaders/font.vert", "shaders/font.frag");

	SDL_GetWindowSize(window, &w, &h);
	glm::mat4 ortho = glm::ortho(0.0f, (float)w, (float)h, 0.0f);

	fontShader->bind();
	GLCALL(glUniformMatrix4fv(glGetUniformLocation(fontShader->getShaderId(), "u_modelViewProj"), 1, GL_FALSE, &ortho[0][0]));


	fontColorUniformIndex = GLCALL(glGetUniformLocation(fontShader->getShaderId(), "u_color"));

	fontShader->unbind();

	MenuItem* newItem;
	newItem = new MenuItem{ true,"Zurueck",MenuItemType::resume };
	menuItemList.push_back(newItem);
	newItem = new MenuItem{ false,"Neustart",MenuItemType::restart };
	menuItemList.push_back(newItem);
	newItem = new MenuItem{ false,"Einstellungen",MenuItemType::options };
	menuItemList.push_back(newItem);
	newItem = new MenuItem{ false,"Beenden",MenuItemType::exit };
	menuItemList.push_back(newItem);
}

void UI::drawFPS(int fps)
{
	drawString(20, 20, std::to_string(fps), glm::vec4(1,1,0,1));
}

void UI::drawPos(Object* object)
{
	drawVec3(object->getPosition(), 1, "Pos", w - 300, 20);
}

void UI::drawRot(Player* object)
{
	drawVec3(object->getLookDirection(), 1, "Rot", w - 300, 50);
}

void UI::drawString(float x, float y, std::string text, glm::vec4 color)
{
	GLCALL(glDisable(GL_CULL_FACE));
	GLCALL(glDisable(GL_DEPTH_TEST));

	fontShader->bind();
	GLCALL(glUniform4fv(fontColorUniformIndex, 1, (float*)&color));

	font->drawString(x, y, text.c_str(), fontShader);
	fontShader->unbind();

	GLCALL(glEnable(GL_CULL_FACE));
	GLCALL(glEnable(GL_DEPTH_TEST));
}

void UI::drawVec3(glm::vec3 vector, int precision, std::string text, float x, float y)
{	
	std::string posString = text+": ";
	drawString(x, y, posString, glm::vec4(1, 1, 1, 1));

	std::stringstream xss, yss, zss;
	xss << std::fixed << std::setprecision(precision) << vector.x;
	posString = xss.str();
	drawString(x+60, y, "x " + posString, glm::vec4(1, 0, 0, 1));

	yss << std::fixed << std::setprecision(precision) << vector.y;
	posString = yss.str();
	drawString(x+140, y, "y " + posString, glm::vec4(0, 1, 0, 1));

	zss << std::fixed << std::setprecision(precision) << vector.z;
	posString = zss.str();
	drawString(x+220, y, "z " + posString, glm::vec4(0, 0, 1, 1));

}

int UI::getHeight()
{
	return h;
}

int UI::getWidth()
{
	return w;
}

void UI::drawUI()
{
	for (UI_Element* ui_text : ui_elements)
	{
		drawString(ui_text->posX, ui_text->posY, ui_text->text, glm::vec4(1, 1, 1, 1));
		delete ui_text;
	}
	ui_elements.clear();
}

void UI::addElement(UI_Element* newElement)
{
	ui_elements.push_back(newElement);
}

void UI::drawMenu()
{
	int x = w / 2 - 50;
	int y = h / 2-menuItemList.size()*40/2;


	for (MenuItem* menuItem : menuItemList)
	{
		if (menuItem->selected)
		{
			drawString(x, y, menuItem->text, glm::vec4(0, 0, 1, 1));
		}
		else
		{
			drawString(x, y, menuItem->text, glm::vec4(1, 1, 1, 1));
		}

		y += 40;
	}
}

void UI::drawPause()
{
	int x = w / 2 - 50;
	int y = h / 2;

	drawString(x, y, "Pause", glm::vec4(1, 1, 1, 1));
}

MenuItem* UI::getSelectedMenuItem()
{
	for (MenuItem* menuItem : menuItemList)
	{
		if (menuItem->selected)
		{
			return menuItem;
		}
	}
}