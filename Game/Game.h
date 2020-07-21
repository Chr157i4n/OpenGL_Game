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

#include <irrKlang.h>


#include "libs/glm/glm.hpp"
#include "libs/glm/ext/matrix_transform.hpp"
#include "libs/glm/gtc/matrix_transform.hpp"

#include "UI.h"
#include "UI_Element_Label.h"
#include "UI_Element_ProgressBar.h"

#include "Renderer.h"

#include "defines.h"

#include "Shader.h"
#include "Object.h"
#include "Player.h"
#include "NPC.h"
#include "Bullet.h"

#include "ResourceManager.h"
#include "ConfigManager.h"

#include "Map.h"




enum GameState {
	GAME_ACTIVE,
	GAME_GAME_OVER,
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
	toggleShowNormals,
	toggleShowShadowMap,
	togglePostprocess,
	pause,
	menu,
	toggleFullscreen,
	enter,
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
	{SDLK_F5,		toggleShowNormals},
	{SDLK_F6,		toggleShowShadowMap},
	{SDLK_F7,		togglePostprocess},
	{SDLK_p,		pause},
	{SDLK_ESCAPE,	menu},
	{SDLK_F11,		toggleFullscreen},	
	{SDLK_RETURN,	enter},
};


static class Game
{
public:

	static void startGame();

	static void init();

	static int getWindowWidth();

	static int getWindowHeight();

	static irrklang::ISoundEngine* SoundEngine;

private:

	static void render();

	static void gameLoop();

	static void processInput();

	static void keyPressed(SDL_Keycode key);

	static void keyReleased(SDL_Keycode key);

	static void deleteObjects();

	static void togglePause();

	static void toggleMenu();

	static void toggleFullscreen();

	static void updateAudioListener();




	static SDL_Window* window;

	static std::vector< std::shared_ptr<Object> > objects;
	static std::vector< std::shared_ptr<Character> > characters;
	static std::vector< std::shared_ptr<Player> > players;
	static std::vector< std::shared_ptr<NPC> > npcs;
	static std::vector< std::shared_ptr<Bullet> > bullets;


	static bool pressedKeys[20];

	static float time;
	static uint32 FPS;
	static bool close;

	static uint64 perfCounterFrequency;
	static uint64 lastCounter;
	static float32 delta;

	static bool showInfo;
	static bool showShadowMap;
	static bool postprocess;

	static GameState gameState;
	static int maxBulletCount;





};