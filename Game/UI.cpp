#include "UI.h"

#include "Game.h"
#include "UI_Element_Label.h"
#include "UI_Element_ProgressBar.h"

Shader* UI::fontShader;
Font* UI::font;
int UI::fontColorUniformIndex;
std::vector<UI_Element*> UI::ui_elements;

UI_Element_Label* UI::fpsLabel;
UI_Element_Label* UI::posLabel[4];
UI_Element_Label* UI::rotLabel[4];

std::list<float> UI::fpsBuffer;

void UI::init()
{
	font = new Font();
	font->initFont("fonts/OpenSans-Regular.ttf");

	fontShader = new Shader("shaders/font.vert", "shaders/font.frag");

	fontShader->bind();

	glm::mat4 ortho = glm::ortho(0.0f, (float)Game::getWindowWidth(), (float)Game::getWindowHeight(), 0.0f);
	GLCALL(glUniformMatrix4fv(glGetUniformLocation(fontShader->getShaderId(), "u_viewProj"), 1, GL_FALSE, &ortho[0][0]));

	glm::mat4 model = glm::mat4(1);
	GLCALL(glUniformMatrix4fv(glGetUniformLocation(fontShader->getShaderId(), "u_model"), 1, GL_FALSE, &model[0][0]));


	fontColorUniformIndex = GLCALL(glGetUniformLocation(fontShader->getShaderId(), "u_color"));

	fontShader->unbind();

	fpsLabel = new UI_Element_Label(20, Game::getWindowHeight() - 20, 10, 10, "", 0, 1, glm::vec4(1, 1, 0, 1), glm::vec4(0.2, 0.2, 0.2, 0.4), true);
	UI::addElement(fpsLabel);


	posLabel[0] = new UI_Element_Label(Game::getWindowWidth()-300, Game::getWindowHeight() - 20, 10, 10, "pos:", 0, 1, glm::vec4(1, 1, 1, 1), glm::vec4(0.2, 0.2, 0.2, 0.4), true);
	UI::addElement(posLabel[0]);
	posLabel[1] = new UI_Element_Label(Game::getWindowWidth()-300+60, Game::getWindowHeight() - 20, 10, 10, "", 0, 1, glm::vec4(1, 0, 0, 1), glm::vec4(0.2, 0.2, 0.2, 0.4), true);
	UI::addElement(posLabel[1]);
	posLabel[2] = new UI_Element_Label(Game::getWindowWidth()-300+140, Game::getWindowHeight() - 20, 10, 10, "", 0, 1, glm::vec4(0, 1, 0, 1), glm::vec4(0.2, 0.2, 0.2, 0.4), true);
	UI::addElement(posLabel[2]);
	posLabel[3] = new UI_Element_Label(Game::getWindowWidth() - 300 + 220, Game::getWindowHeight() - 20, 10, 10, "", 0, 1, glm::vec4(0, 0, 1, 1), glm::vec4(0.2, 0.2, 0.2, 0.4), true);
	UI::addElement(posLabel[3]);

	rotLabel[0] = new UI_Element_Label(Game::getWindowWidth() - 300, Game::getWindowHeight() - 50, 10, 10, "rot:", 0, 1, glm::vec4(1, 1, 1, 1), glm::vec4(0.2, 0.2, 0.2, 0.4), true);
	UI::addElement(rotLabel[0]);
	rotLabel[1] = new UI_Element_Label(Game::getWindowWidth() - 300 + 60, Game::getWindowHeight() - 50, 10, 10, "", 0, 1, glm::vec4(1, 0, 0, 1), glm::vec4(0.2, 0.2, 0.2, 0.4), true);
	UI::addElement(rotLabel[1]);
	rotLabel[2] = new UI_Element_Label(Game::getWindowWidth() - 300 + 140, Game::getWindowHeight() - 50, 10, 10, "", 0, 1, glm::vec4(0, 1, 0, 1), glm::vec4(0.2, 0.2, 0.2, 0.4), true);
	UI::addElement(rotLabel[2]);
	rotLabel[3] = new UI_Element_Label(Game::getWindowWidth() - 300 + 220, Game::getWindowHeight() - 50, 10, 10, "", 0, 1, glm::vec4(0, 0, 1, 1), glm::vec4(0.2, 0.2, 0.2, 0.4), true);
	UI::addElement(rotLabel[3]);
}

void UI::updateFPS()
{
	fpsBuffer.push_back(Game::getFPS());
	if (fpsBuffer.size() > 10)
	{
		fpsBuffer.pop_front();
	}

	float fpsAverage = 0;
	for (float fps : fpsBuffer)
	{
		fpsAverage += fps;
	}
	fpsAverage /= 10.0;

	fpsLabel->setText(std::to_string((int)std::round(fpsAverage)));

	if (fpsAverage < 30)
	{
		fpsLabel->setForeColor(glm::vec4(1, 0, 0, 1));
	}
	else if(fpsAverage < 80)
	{
		fpsLabel->setForeColor(glm::vec4(1, 1, 0, 1));
	}
	else
	{
		fpsLabel->setForeColor(glm::vec4(0, 0, 1, 1));
	}
}

void UI::updatePos(std::shared_ptr<Object> object)
{
	int precision = 2;
	std::string text;
	std::stringstream xss, yss, zss;

	xss << std::fixed << std::setprecision(precision) << object->getPosition().x;
	text = xss.str();
	posLabel[1]->setText(text);

	yss << std::fixed << std::setprecision(precision) << object->getPosition().y;
	text = yss.str();
	posLabel[2]->setText(text);

	zss << std::fixed << std::setprecision(precision) << object->getPosition().z;
	text = zss.str();
	posLabel[3]->setText(text);
}

void UI::updateRot(std::shared_ptr<Player> object)
{
	int precision = 2;
	std::string text;
	std::stringstream xss, yss, zss;

	xss << std::fixed << std::setprecision(precision) << object->getLookDirection().x;
	text = xss.str();
	rotLabel[1]->setText(text);

	yss << std::fixed << std::setprecision(precision) << object->getLookDirection().y;
	text = yss.str();
	rotLabel[2]->setText(text);

	zss << std::fixed << std::setprecision(precision) << object->getLookDirection().z;
	text = zss.str();
	rotLabel[3]->setText(text);
}

void UI::drawString(float x, float y, std::string text, glm::vec4 color)//, glm::vec3 scale)
{
	fontShader->bind();

	GLCALL(glUniform4fv(fontColorUniformIndex, 1, (float*)&color));

	//glm::mat4 model = glm::mat4(1);
	//model = glm::scale(model, scale);
	//GLCALL(glUniformMatrix4fv(glGetUniformLocation(fontShader->getShaderId(), "u_model"), 1, GL_FALSE, &model[0][0]));

	font->drawString(x, y, text.c_str(), fontShader);
	fontShader->unbind();
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

	GLCALL(glDisable(GL_CULL_FACE));
	GLCALL(glDisable(GL_DEPTH_TEST));
	drawString(x, y, "Pause", glm::vec4(1, 1, 1, 1));
	GLCALL(glEnable(GL_CULL_FACE));
	GLCALL(glEnable(GL_DEPTH_TEST));
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