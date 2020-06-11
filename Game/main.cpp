#include "windows.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#define GLEW_STATIC
#include <GL/glew.h>
#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>     /* srand, rand */
#include <time.h> 

#include "libs/glm/glm.hpp"
#include "libs/glm/ext/matrix_transform.hpp"
#include "libs/glm/gtc/matrix_transform.hpp"

#define STB_IMAGE_IMPLEMENTATION
//#include "libs/stb_image.h"

#pragma comment(lib, "SDL2.lib")
#pragma comment(lib, "glew32s.lib")
#pragma comment(lib, "opengl32.lib")

void openGLDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
	std::cout << "[OpenGL Error] " << message << std::endl;
}

#ifdef _DEBUG

void _GLGetError(const char* file, int line, const char* call) {
	while (GLenum error = glGetError()) {
		std::cout << "[OpenGL Error] " << glewGetErrorString(error) << " in " << file << ":" << line << " Call: " << call << std::endl;
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



int main(int argc, char* argv[])
{
	ConfigManager configManager("config.ini");
	ResourceManager resourceManager(&configManager);

	HWND console;
	AllocConsole();
	console = FindWindowA("ConsoleWindowClass", NULL);
	ShowWindow(console, 1);


	SDL_Window* window;
	SDL_Init(SDL_INIT_EVERYTHING);

	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetSwapInterval(1);

	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 8);

	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);


	#ifdef _DEBUG
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
	#endif // DEBUG

	uint32 flags = SDL_WINDOW_OPENGL;
	int fullscreen = std::stoi(configManager.readConfig("fullscreen"));
	if (fullscreen == 1)
	{
		flags += SDL_WINDOW_FULLSCREEN_DESKTOP;
	}

	int resolution_width = std::stoi(configManager.readConfig("resolution_width"));
	int resolution_height = std::stoi(configManager.readConfig("resolution_height"));

	window = SDL_CreateWindow("OpenGL-Test", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, resolution_width, resolution_height, flags);
	SDL_GLContext glContext = SDL_GL_CreateContext(window);

	GLenum err = glewInit();
	if (err != GLEW_OK)
	{
		std::cout << "Error" << glewGetErrorString(err) << std::endl;
		std::cin.get();
		return -1;
	}
	std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;

#ifdef _DEBUG
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(openGLDebugCallback, 0);
#endif // DEBUG

	srand(time(NULL));




	std::vector<Object*> objects;
	std::vector<Player*> players;
	std::vector<NPC*> npcs;
	std::vector<Bullet*> bullets;

	resourceManager.loadShader("shaders/basic.vert", "shaders/basic.frag");

	
	int directionLocation = GLCALL(glGetUniformLocation(resourceManager.getShader()->getShaderId(), "u_directional_light.direction"));
	glm::vec3 sunColor = glm::vec3(0.98f,0.83f,0.25f);
	//sunColor *= 0.4f;
	glm::vec3 sunDirection = glm::vec3(-1.0f);
	GLCALL(glUniform3fv(glGetUniformLocation(resourceManager.getShader()->getShaderId(), "u_directional_light.diffuse"), 1, (float*)&sunColor));
	GLCALL(glUniform3fv(glGetUniformLocation(resourceManager.getShader()->getShaderId(), "u_directional_light.specular"), 1, (float*)&sunColor));
	sunColor *= 0.2f;
	GLCALL(glUniform3fv(glGetUniformLocation(resourceManager.getShader()->getShaderId(), "u_directional_light.ambient"), 1, (float*)&sunColor));


	glm::vec3 pointLightColor = glm::vec3(0,0,1);
	GLCALL(glUniform3fv(glGetUniformLocation(resourceManager.getShader()->getShaderId(), "u_point_light.diffuse"), 1, (float*)&pointLightColor));
	GLCALL(glUniform3fv(glGetUniformLocation(resourceManager.getShader()->getShaderId(), "u_point_light.specular"), 1, (float*)&pointLightColor));
	pointLightColor *= 0.2f;
	GLCALL(glUniform3fv(glGetUniformLocation(resourceManager.getShader()->getShaderId(), "u_point_light.ambient"), 1, (float*)&pointLightColor));
	GLCALL(glUniform1f(glGetUniformLocation(resourceManager.getShader()->getShaderId(), "u_point_light.linear"), 0.007f));
	GLCALL(glUniform1f(glGetUniformLocation(resourceManager.getShader()->getShaderId(), "u_point_light.quadratic"), 0.0002));
	glm::vec4 pointLightPosition = glm::vec4(80, 2.5, 10, 1.0f);
	int positionLocation = GLCALL(glGetUniformLocation(resourceManager.getShader()->getShaderId(), "u_point_light.position"));


	glm::vec3 spotLightColor = glm::vec3(0);
	GLCALL(glUniform3fv(glGetUniformLocation(resourceManager.getShader()->getShaderId(), "u_spot_light.diffuse"), 1, (float*)&spotLightColor));
	GLCALL(glUniform3fv(glGetUniformLocation(resourceManager.getShader()->getShaderId(), "u_spot_light.specular"), 1, (float*)&spotLightColor));
	spotLightColor *= 0.2f;
	GLCALL(glUniform3fv(glGetUniformLocation(resourceManager.getShader()->getShaderId(), "u_spot_light.ambient"), 1, (float*)&spotLightColor));
	glm::vec3 spotLightPosition = glm::vec3(0.0f);
	GLCALL(glUniform3fv(glGetUniformLocation(resourceManager.getShader()->getShaderId(), "u_spot_light.position"), 1, (float*)&spotLightPosition));
	spotLightPosition.z = 1.0f;
	GLCALL(glUniform3fv(glGetUniformLocation(resourceManager.getShader()->getShaderId(), "u_spot_light.direction"), 1, (float*)&spotLightPosition));
	GLCALL(glUniform1f(glGetUniformLocation(resourceManager.getShader()->getShaderId(), "u_spot_light.innerCone"), 0.95f));
	GLCALL(glUniform1f(glGetUniformLocation(resourceManager.getShader()->getShaderId(), "u_spot_light.outerCone"), 0.80f));


	std::string levelname = configManager.readConfig("level");
	resourceManager.loadMap("levels/"+ levelname, &objects, &players, &npcs);


	


	uint64 perfCounterFrequency = SDL_GetPerformanceFrequency();
	uint64 lastCounter = SDL_GetPerformanceCounter();
	float32 delta = 0.0f;

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::scale(model, glm::vec3(1.0f));



	glm::mat4 modelViewProj = players[0]->getViewProj() * model;


	int modelViewProjMatrixUniformIndex = GLCALL(glGetUniformLocation(resourceManager.getShader()->getShaderId(), "u_modelViewProj"));
	int modelViewUniformIndex = GLCALL(glGetUniformLocation(resourceManager.getShader()->getShaderId(), "u_modelView"));
	int invmodelViewUniformIndex = GLCALL(glGetUniformLocation(resourceManager.getShader()->getShaderId(), "u_invModelView"));


	GLCALL(glEnable(GL_CULL_FACE));
	GLCALL(glEnable(GL_DEPTH_TEST));
	GLCALL(glFrontFace(GL_CCW));

	bool buttonW = false;
	bool buttonS = false;
	bool buttonA = false;
	bool buttonD = false;
	bool buttonSpace = false;
	bool buttonShift = false;
	bool buttonCtrl = false;

	bool spaceReleased = true;

	float time = 0.0f;
	bool close = false;

	SDL_SetRelativeMouseMode(SDL_TRUE);


	

	while (!close)
	{
		time += delta;
		GLCALL(glClearColor(0.0f, 0.0f, 0.8f, 1.0f));
		GLCALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
		

		#pragma region inputs
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
			{
				close = true;
			}
			else if (event.type == SDL_KEYDOWN)
			{
				switch (event.key.keysym.sym)
				{
				case SDLK_w:
					buttonW = true;
					break;
				case SDLK_s:
					buttonS = true;
					break;
				case SDLK_a:
					buttonA = true;
					break;
				case SDLK_d:
					buttonD = true;
					break;
				case SDLK_SPACE:
					buttonSpace = true;
					break;
				case SDLK_LCTRL:
					buttonCtrl = true;
					break;
				case SDLK_LSHIFT:
					buttonShift = true;
					break;
				case SDLK_p:
					GLCALL(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE));
					break;
				case SDLK_f:
					glm::vec3 spotLightColor = glm::vec3(1.0f);
					GLCALL(glUniform3fv(glGetUniformLocation(resourceManager.getShader()->getShaderId(), "u_spot_light.diffuse"), 1, (float*)&spotLightColor));
					GLCALL(glUniform3fv(glGetUniformLocation(resourceManager.getShader()->getShaderId(), "u_spot_light.specular"), 1, (float*)&spotLightColor));
					spotLightColor *= 0.2f;
					GLCALL(glUniform3fv(glGetUniformLocation(resourceManager.getShader()->getShaderId(), "u_spot_light.ambient"), 1, (float*)&spotLightColor));
					break;
				case SDLK_ESCAPE:
					SDL_SetRelativeMouseMode(SDL_FALSE);
					break;


				}
			}
			else if (event.type == SDL_KEYUP)
			{
				switch (event.key.keysym.sym)
				{
				case SDLK_w:
					buttonW = false;
					break;
				case SDLK_s:
					buttonS = false;
					break;
				case SDLK_a:
					buttonA = false;
					break;
				case SDLK_d:
					buttonD = false;
					break;
				case SDLK_SPACE:
					buttonSpace = false;
					spaceReleased = true;
					break;
				case SDLK_LSHIFT:
					buttonShift = false;
					break;
				case SDLK_LCTRL:
					buttonCtrl = false;
					break;
				case SDLK_p:
					GLCALL(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));
					break;
				case SDLK_f:
					glm::vec3 spotLightColor = glm::vec3(0);
					GLCALL(glUniform3fv(glGetUniformLocation(resourceManager.getShader()->getShaderId(), "u_spot_light.diffuse"), 1, (float*)&spotLightColor));
					GLCALL(glUniform3fv(glGetUniformLocation(resourceManager.getShader()->getShaderId(), "u_spot_light.specular"), 1, (float*)&spotLightColor));
					GLCALL(glUniform3fv(glGetUniformLocation(resourceManager.getShader()->getShaderId(), "u_spot_light.ambient"), 1, (float*)&spotLightColor));
					break;
				}
			}
			else if (event.type == SDL_MOUSEMOTION) {
				if (SDL_GetRelativeMouseMode()) {
					players[0]->onMouseMove(event.motion.xrel, event.motion.yrel);
				}
			}
			else if (event.type == SDL_MOUSEBUTTONDOWN) {
				if (event.button.button == SDL_BUTTON_LEFT) {
					SDL_SetRelativeMouseMode(SDL_TRUE);
					Bullet* newBullet = players[0]->shoot();
					objects.push_back(newBullet);
					bullets.push_back(newBullet);
				}
			}

		}

		if (buttonW) {
			players[0]->moveForward(objects);
		}
		if (buttonS) {
			players[0]->moveBackward(objects);
		}
		if (buttonA) {
			players[0]->moveLeft(objects);
		}
		if (buttonD) {
			players[0]->moveRight(objects);
		}
		if (buttonSpace && spaceReleased) {
			players[0]->jump();
			spaceReleased = false;
		}
		if (buttonCtrl) {
			players[0]->crouch(true);
		}
		if (!buttonCtrl) {
			players[0]->crouch(false);
		}
		if (buttonShift) {
			players[0]->run(true);
		}
		if (!buttonShift) {
			players[0]->run(false);
		}
		
		#pragma endregion

		for (NPC* npc : npcs)
		{
			npc->followCharacter(delta / 1000, players[0]);
		}

		for (Object* object : objects)
		{
			object->move(delta/ 1000);
			object->fall(delta / 1000);
		}
		for (Player* player : players)
		{
			player->updateCameraPosition();
			player->update();
		}


		glm::vec4 transformedSunDirection = glm::transpose(glm::inverse(players[0]->getView())) * glm::vec4(sunDirection, 1.0f);
		glUniform3fv(directionLocation, 1, (float*)&transformedSunDirection);

		glm::mat4 pointLightMatrix = glm::mat4(1.0f);
		pointLightPosition = pointLightMatrix * pointLightPosition;
		glm::vec3 transformedPointLightPosition = (glm::vec3) (players[0]->getView() * pointLightPosition);
		glUniform3fv(positionLocation, 1, (float*)&transformedPointLightPosition);

		for (int i = 0; i < objects.size(); i++)
		{
			glm::mat4 model = glm::mat4(1.0f);
			//model = glm::scale(model, glm::vec3(1.0f));

			//move to position of model
			model = glm::translate(model, objects[i]->getPosition());
			
			//rotate model around X
			float angle = objects[i]->getRotation().x;
			model = glm::rotate(model, glm::radians(angle), glm::vec3(1, 0, 0));

			//rotate model around Y
			angle = objects[i]->getRotation().y;
			model = glm::rotate(model, glm::radians(angle), glm::vec3(0, 1, 0));

			//rotate model around z
			angle = objects[i]->getRotation().z;
			model = glm::rotate(model, glm::radians(angle), glm::vec3(0, 0, 1));

			//view and projection
			modelViewProj = players[0]->getViewProj() * model;
			glm::mat4 modelView = players[0]->getView() * model;
			glm::mat4 invModelView = glm::transpose(glm::inverse(modelView));


			GLCALL(glUniformMatrix4fv(modelViewProjMatrixUniformIndex, 1, GL_FALSE, &modelViewProj[0][0]));
			GLCALL(glUniformMatrix4fv(modelViewUniformIndex, 1, GL_FALSE, &modelView[0][0]));
			GLCALL(glUniformMatrix4fv(invmodelViewUniformIndex, 1, GL_FALSE, &invModelView[0][0]));

			objects[i]->render();
		}

		SDL_GL_SwapWindow(window);

		//Sleep(100);

		uint64 endCounter = SDL_GetPerformanceCounter();
		uint64 counterElasped = endCounter - lastCounter;
		delta = ((float32)counterElasped) / (float32)perfCounterFrequency;
		uint32 FPS = (uint32)((float32)perfCounterFrequency / (float32)counterElasped);
		//std::cout << "FPS: " << FPS << std::endl;
		lastCounter = endCounter;
	}


	return 0;
};