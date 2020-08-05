#include "UI.h"

#include "Game.h"
#include "UI_Element_Label.h"
#include "UI_Element_ProgressBar.h"

Shader* UI::fontShader;
Font* UI::font;
int UI::fontColorUniformIndex;
std::vector<UI_Element*> UI::ui_elements;

void UI::init()
{

	font = new Font();
	font->initFont("fonts/OpenSans-Regular.ttf");

	fontShader = new Shader("shaders/font.vert", "shaders/font.frag");

	glm::mat4 ortho = glm::ortho(0.0f, (float)Game::getWindowWidth(), (float)Game::getWindowHeight(), 0.0f);

	fontShader->bind();
	GLCALL(glUniformMatrix4fv(glGetUniformLocation(fontShader->getShaderId(), "u_modelViewProj"), 1, GL_FALSE, &ortho[0][0]));


	fontColorUniformIndex = GLCALL(glGetUniformLocation(fontShader->getShaderId(), "u_color"));

	fontShader->unbind();
}

void UI::drawFPS()
{
	drawString(20, Game::getWindowHeight() - 20, std::to_string((int)std::round(Game::getFPS())), glm::vec4(1,1,0,1));
}

void UI::drawPos(std::shared_ptr<Object> object)
{
	drawVec3(object->getPosition(), 1, "Pos", Game::getWindowWidth() - 300, Game::getWindowHeight() - 20);
}

void UI::drawRot(std::shared_ptr<Player> object)
{
	drawVec3(object->getLookDirection(), 1, "Rot", Game::getWindowWidth() - 300, Game::getWindowHeight() - 50);
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

void UI::drawUI()
{
	GLCALL(glDepthMask(GL_FALSE));
	GLCALL(glDisable(GL_CULL_FACE));
	GLCALL(glDisable(GL_DEPTH_TEST));

	for (UI_Element* ui_element : ui_elements)
	{
		if (!ui_element->getIsDebugInfo() || Game::getShowInfo())
		{
			ui_element->drawUI_Element();
		}
	}
	checkLifeSpan();

	GLCALL(glDepthMask(GL_TRUE));
	GLCALL(glEnable(GL_CULL_FACE));
	GLCALL(glEnable(GL_DEPTH_TEST));
}

void UI::addElement(UI_Element* newElement)
{
	for (UI_Element* ui_element : ui_elements)
	{
		if (newElement->getX() == ui_element->getX() && newElement->getY() == ui_element->getY()) return;
	}
	
	newElement->setID(ui_elements.size());
	ui_elements.push_back(newElement);
}

void UI::drawPause()
{
	int x = Game::getWindowWidth() / 2 - 50;
	int y = Game::getWindowHeight() / 2;

	drawString(x, y, "Pause", glm::vec4(1, 1, 1, 1));
}

void UI::checkLifeSpan()
{
	for(int i=ui_elements.size()-1; i>=0; i--)
	{
		if (!ui_elements[i]->isStillAlive())
		{
			auto it = std::find(ui_elements.begin(), ui_elements.end(), ui_elements[i]);
			if (it != ui_elements.end()) { ui_elements.erase(it); }
		}
	}
}

void UI::clearMessages()
{
	for (int i = ui_elements.size() - 1; i >= 0; i--)
	{
		if (ui_elements[i]->getUielementtype() == UI_Element_Type::label)
		{
			auto it = std::find(ui_elements.begin(), ui_elements.end(), ui_elements[i]);
			if (it != ui_elements.end()) { ui_elements.erase(it); }
		}
	}
}

void UI::clearUI_Elements()
{
	for (int i = ui_elements.size() - 1; i >= 0; i--)
	{
		if (ui_elements[i]->getLifespan() > 0)	//delete temporal elements
		{
			auto it = std::find(ui_elements.begin(), ui_elements.end(), ui_elements[i]);
			if (it != ui_elements.end()) { ui_elements.erase(it); }
			continue;
		}

		if (!ui_elements[i]->getIsDebugInfo())	//Palyer associated
		{
			auto it = std::find(ui_elements.begin(), ui_elements.end(), ui_elements[i]);
			if (it != ui_elements.end()) { ui_elements.erase(it); }
			continue;
		}

	}
}

float UI::to_x_gl(int x_px)
{
	return ((float)x_px) / Game::getWindowWidth() * 2 -1;
}

float UI::to_y_gl(int y_px)
{
	return ((float)y_px) / Game::getWindowHeight() * 2 -1;
}

int UI::to_x_px(float x_gl)
{
	return (x_gl+1) * Game::getWindowWidth() / 2;
}

int UI::to_y_px(float y_gl)
{
	return (y_gl+1) * Game::getWindowHeight() / 2;
}


float UI::to_w_gl(int w_px)
{
	return ((float)w_px) / Game::getWindowWidth() * 2;
}

float UI::to_h_gl(int h_px)
{
	return ((float)h_px) / Game::getWindowHeight() * 2;
}

int UI::to_w_px(float w_gl)
{
	return w_gl * Game::getWindowWidth() / 2;
}

int UI::to_h_px(float h_gl)
{
	return h_gl * Game::getWindowHeight() / 2;
}