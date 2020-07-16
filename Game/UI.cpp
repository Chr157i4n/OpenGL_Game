#include "UI.h"


SDL_Window* UI::window;
Shader* UI::fontShader;
Font* UI::font;
int UI::w, UI::h;
std::vector<UI_Element*> UI::ui_elements;

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
	fontShader->unbind();
}

void UI::drawFPS(int fps)
{
	drawString(20, 20, std::to_string(fps));
}

void UI::drawPos(Object* object)
{
	std::string posString = "Pos: ";

	std::stringstream xss, yss, zss;
	xss << std::fixed << std::setprecision(1) << object->getPosition().x;
	std::string xPosString = xss.str();

	yss << std::fixed << std::setprecision(1) << object->getPosition().y;
	std::string yPosString = yss.str();

	zss << std::fixed << std::setprecision(1) << object->getPosition().z;
	std::string zPosString = zss.str();

	posString += "x " + xPosString + ", ";
	posString += "y " + yPosString + ", ";
	posString += "z " + zPosString;

	drawString(w - 300, 20, posString);
}

void UI::drawRot(Player* object)
{
	std::string rotString = "Rot: ";

	std::stringstream xss, yss, zss;
	xss << std::fixed << std::setprecision(1) << object->getLookDirection().x;
	std::string xRotString = xss.str();

	yss << std::fixed << std::setprecision(1) << object->getLookDirection().y;
	std::string yRotString = yss.str();

	zss << std::fixed << std::setprecision(1) << object->getLookDirection().z;
	std::string zRotString = zss.str();

	rotString += "x " + xRotString + ", ";
	rotString += "y " + yRotString + ", ";
	rotString += "z " + zRotString;

	drawString(w - 300, 50, rotString);
}

void UI::drawString(float x, float y, std::string text)
{
	GLCALL(glDisable(GL_CULL_FACE));
	GLCALL(glDisable(GL_DEPTH_TEST));

	fontShader->bind();
	font->drawString(x, y, text.c_str(), fontShader);
	fontShader->unbind();

	GLCALL(glEnable(GL_CULL_FACE));
	GLCALL(glEnable(GL_DEPTH_TEST));
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
		drawString(ui_text->posX, ui_text->posY, ui_text->text);
		delete ui_text;
	}
	ui_elements.clear();
}

void UI::addElement(UI_Element* newElement)
{
	ui_elements.push_back(newElement);
}