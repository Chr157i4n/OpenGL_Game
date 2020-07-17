#pragma once

#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>     /* srand, rand */
#include <time.h> 
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>

//#define GLEW_STATIC
#include <GL/glew.h>
#define SDL_MAIN_HANDLED
#include <SDL.h>


#include "libs/glm/glm.hpp"
#include "libs/glm/ext/matrix_transform.hpp"
#include "libs/glm/gtc/matrix_transform.hpp"

#include "UI.h"
#include "Renderer.h"

#include "defines.h"

#include "Shader.h"
#include "Object.h"
#include "Player.h"
#include "NPC.h"
#include "Bullet.h"

#include "ResourceManager.h"
#include "ConfigManager.h"




enum GameState {
	GAME_ACTIVE,
	GAME_PAUSED,
	GAME_MENU
};

enum PlayerAction {
	None,
	moveForward,
	moveBackward,
	moveLeft,
	moveRight,
	jump,
	sprint,
	crouch,
	toggleFlashlight,
	toggleInfo,
	toggleWireframe,
	pause,
	menu,
};

std::unordered_map<SDL_Keycode, PlayerAction> const keybindings =
{
	{SDLK_w,		moveForward},
	{SDLK_s,		moveBackward},
	{SDLK_a,		moveLeft},
	{SDLK_d,		moveRight},
	{SDLK_SPACE,	jump},
	{SDLK_LSHIFT,	sprint},
	{SDLK_LCTRL,	crouch},
	{SDLK_f,		toggleFlashlight},
	{SDLK_F3,		toggleInfo},
	{SDLK_F4,		toggleWireframe},
	{SDLK_p,		pause},
	{SDLK_ESCAPE,	menu},
};


static class Game
{
public:

	static void startGame();

	static void init();


private:

	static void render();

	static void gameLoop();

	static void processInput();

	static void keyPressed(SDL_Keycode key);

	static void keyReleased(SDL_Keycode key);

	static void deleteObjects();



	static SDL_Window* window;

	static std::vector<Object*> objects;
	static std::vector<Character*> characters;
	static std::vector<Player*> players;
	static std::vector<NPC*> npcs;
	static std::vector<Bullet*> bullets;


	static bool pressedKeys[20];

	static float time;
	static uint32 FPS;
	static bool close;

	static uint64 perfCounterFrequency;
	static uint64 lastCounter;
	static float32 delta;




};