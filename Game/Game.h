#pragma once

#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>     /* srand, rand */
#include <time.h> 
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#define GLEW_STATIC
#include <GL/glew.h>
#define SDL_MAIN_HANDLED
#include <SDL.h>

//#define STB_IMAGE_IMPLEMENTATION
//#include "libs/stb_image.h"
//#undef STB_IMAGE_IMPLEMENTATION

#include "libs/glm/glm.hpp"
#include "libs/glm/ext/matrix_transform.hpp"
#include "libs/glm/gtc/matrix_transform.hpp"


#include "Logger.h"

#ifdef _DEBUG123
void openGLDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
	std::string messageS = message;
	Logger::log("[OpenGL Callback] " + messageS);
}

void _GLGetError(const char* file, int line, const char* call) {
	while (GLenum error = glGetError())
	{
		Logger::log("[OpenGL Error] " + std::string((char*)glewGetErrorString(error)) + " in " + std::string(file) + ":" + std::to_string(line) + " Call: " + std::string(call));
	}
}
#define GLCALL(call) call; _GLGetError(__FILE__, __LINE__, #call)
#else
#define GLCALL(call) call
#endif

#include "defines.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Shader.h"
#include "FloatingCamera.h"
#include "FPSCamera.h"
#include "Camera.h"
#include "Object.h"
#include "Player.h"
#include "NPC.h"
#include "ResourceManager.h"
#include "ConfigManager.h"
#include "Bullet.h"
#include "UI.h"


static class Game
{
public:

	static void startGame();

	static void initOpenGL();

	static void initEverythingElse();

private:

	static void gameLoop();

	static void input();


	static SDL_Window* window;

	static std::vector<Object*> objects;
	static std::vector<Player*> players;
	static std::vector<NPC*> npcs;
	static std::vector<Bullet*> bullets;


	static bool buttonW;
	static bool buttonS;
	static bool buttonA;
	static bool buttonD;
	static bool buttonSpace;
	static bool buttonShift;
	static bool buttonCtrl;

	static bool spaceReleased;

	static float time;
	static uint32 FPS;
	static bool close;

	static uint64 perfCounterFrequency;
	static uint64 lastCounter;
	static float32 delta;

	static int modelViewProjMatrixUniformIndex;
	static int modelViewUniformIndex;
	static int invmodelViewUniformIndex;

	static int directionLocation;

	static glm::vec3 sunDirection;
	static glm::vec3 spotLightPosition;

	static int positionLocation;

	static glm::vec4 pointLightPosition;

	static glm::mat4 modelViewProj;

};