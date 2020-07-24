#pragma once
#include "defines.h"

#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>     /* srand, rand */
#include <time.h> 
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <memory>

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
#include "UI_Element_Graph.h"

#include "Renderer.h"

#include "Shader.h"
#include "Object.h"
#include "Player.h"
#include "NPC.h"
#include "Bullet.h"

#include "ResourceManager.h"
#include "ConfigManager.h"

#include "Map.h"


//#define DEBUG_COLLISION
//#define DEBUG_OUTOFBOUNDS
//#define DEBUG_GRAVITY
//#define DEBUG_NPC

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
	interact,
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
	toggleConsole,
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
	{SDLK_e,		interact},
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
	{SDLK_CARET,	toggleConsole},
};


static class Game
{
public:

	static void startGame();

	static void init();

	static int getWindowWidth();

	static int getWindowHeight();

	static float32 getTimestamp();

	/// <summary>
	/// returns the delta time between this and the last frame in seconds
	/// </summary>
	/// <returns>delta time in s</returns>
	static float32 getDelta()
	{
		return delta;
	}

	static float32 getFPS()
	{
		return FPS;
	}

	static void setGameState(GameState newGameState)
	{
		gameState = newGameState;
	}

	static GameState getGameState()
	{
		return gameState;
	}

	static bool getShowInfo()
	{
		return showInfo;
	}

	static std::vector< std::shared_ptr<Object> > map;
	static std::vector< std::shared_ptr<Object> > objects;
	static std::vector< std::shared_ptr<Character> > characters;
	static std::vector< std::shared_ptr<Player> > players;
	static std::vector< std::shared_ptr<NPC> > npcs;
	static std::vector< std::shared_ptr<Bullet> > bullets;

	static SDL_Window* window;

	static irrklang::ISoundEngine* SoundEngine;

private:

	static void processCollision();

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

	static void openConsole();



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