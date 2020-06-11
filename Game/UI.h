#pragma once

#include <iomanip>
#include <sstream>
#include <SDL.h>
#include <cstring>
#include <string>

#include "Font.h"
#include "Shader.h"
#include "Object.h"


class UI
{
public:

	UI(SDL_Window* window)
	{
		this->window = window;

		font = new Font();
		font->initFont("fonts/OpenSans-Regular.ttf");

		fontShader = new Shader("shaders/font.vert", "shaders/font.frag");
		
		SDL_GetWindowSize(window, &w, &h);
		glm::mat4 ortho = glm::ortho(0.0f, (float)w, (float)h, 0.0f);

		fontShader->bind();
		GLCALL(glUniformMatrix4fv(glGetUniformLocation(fontShader->getShaderId(), "u_modelViewProj"), 1, GL_FALSE, &ortho[0][0]));	
		fontShader->unbind();
	}

	void drawFPS(int fps)
	{
		drawString(20, 20, std::to_string(fps));
	}

	void drawPos(Object* object)
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

		drawString(w-300, 20, posString);
	}

	void drawString(float x, float y, std::string text)
	{
		GLCALL(glDisable(GL_CULL_FACE));
		GLCALL(glDisable(GL_DEPTH_TEST));

		fontShader->bind();
		font->drawString(x, y, text.c_str(), fontShader);
		fontShader->unbind();
		
		GLCALL(glEnable(GL_CULL_FACE));
		GLCALL(glEnable(GL_DEPTH_TEST));
	}

private:
	SDL_Window* window;
	Shader* fontShader;
	Font* font;

	int w, h;

};

