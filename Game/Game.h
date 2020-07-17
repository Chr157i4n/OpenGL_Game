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



#include "libs/glm/glm.hpp"
#include "libs/glm/ext/matrix_transform.hpp"
#include "libs/glm/gtc/matrix_transform.hpp"


#include "Logger.h"



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
#include "Renderer.h"





static class Game
{
public:

	static void startGame();

	static void init();


private:

	static void render();

	static void gameLoop();

	static void input();

	static void deleteObjects();


	static SDL_Window* window;

	static std::vector<Object*> objects;
	static std::vector<Character*> characters;
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




};