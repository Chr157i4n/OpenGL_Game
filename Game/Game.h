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
#include <chrono>
#include <thread>

//#define GLEW_STATIC
#include <GL/glew.h>
#define SDL_MAIN_HANDLED
#include <SDL.h>

#include <irrKlang.h>

#include "libs/glm/glm.hpp"
#include "libs/glm/ext/matrix_transform.hpp"
#include "libs/glm/gtc/matrix_transform.hpp"

//Menu
#include "Menu.h"
#include "Menu_Main.h"
#include "Menu_Options.h"
#include "Menu_Pause.h"

//UI
#include "UI.h"
#include "UI_Element_Label.h"
#include "UI_Element_ProgressBar.h"
#include "UI_Element_Graph.h"

//Game Graphics
#include "Renderer.h"
#include "Shader.h"

//Game Logic
#include "Object.h"
#include "Player.h"
#include "NPC.h"
#include "Bullet.h"

//Resourcemanagment
#include "ResourceManager.h"
#include "ConfigManager.h"
#include "Map.h"


//Utilitiy
#include "StopWatch.h"


//#define DEBUG_COLLISION
//#define DEBUG_OUTOFBOUNDS
//#define DEBUG_GRAVITY
//#define DEBUG_NPC

enum GameState {
	GAME_LOADING,
	GAME_ACTIVE,
	GAME_GAME_OVER,
	GAME_PAUSED,
	GAME_MENU,
};

enum MenuType {
	MENU_MAIN,
	MENU_OPTIONS,
	MENU_PAUSE,
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

enum MouseAction {
	lmb,
	mmb,
	rmb,
	x1mb,
	x2mb,
};

std::unordered_map<Uint8, MouseAction> const mousebindings =
{
	{SDL_BUTTON_LEFT,		lmb},
	{SDL_BUTTON_MIDDLE,		mmb},
	{SDL_BUTTON_RIGHT,		rmb},
	{SDL_BUTTON_X1,			x1mb},
	{SDL_BUTTON_X2,			x2mb},
};

static class Game
{
public:

	static void startGame();

	static void init();

	static int getWindowWidth();

	static int getWindowHeight();

	static float32 getDelta()
	{
		if (delta < 100)
		{
			return delta;
		}
		else
		{
			return 100;
		}
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

	static void togglePause();

	static bool toggleMenu();

	static bool toggleSubMenu(MenuType submenu);

	static void toggleFullscreen();

	static void toggleFullscreen(FullscreenOption option);

	static void quit()
	{
		ConfigManager::writeAllConfigs();
		SDL_DestroyWindow(Game::window);
		exit(0);
	}

	static void swapBuffer()
	{
		SDL_GL_SwapWindow(window);
	}

	static void changeSize(int w, int h);


	static std::vector< std::shared_ptr<Object> > map;
	static std::vector< std::shared_ptr<Object> > objects;
	static std::vector< std::shared_ptr<Character> > characters;
	static std::vector< std::shared_ptr<Player> > players;
	static std::vector< std::shared_ptr<NPC> > npcs;
	static std::vector< std::shared_ptr<Bullet> > bullets;

	static SDL_Window* window;

	static irrklang::ISoundEngine* SoundEngine;

	static bool pressedKeys[40];
	static bool pressedMouseButtons[6];

	static float32 FPS;
	static float32 fps_limit_ingame;
	static float32 fps_limit_menu;
	static bool close;


	static float32 delta;

	static bool showInfo;
	static bool showShadowMap;
	static bool postprocess;

	static GameState gameState;
	static GameState newGameState;
	static int maxBulletCount;

	static Menu* menu_Main;
	static Menu* menu_Pause;
	static Menu* menu_Options;

	static Menu* menu_current;
	static Menu* menu_last;

private:

	static void processCollision();

	static void render();

	static void gameLoop();

	static void processInput();

	static void keyPressed(SDL_Keycode key);

	static void keyReleased(SDL_Keycode key);

	static void deleteObjects();

	static void updateAudioListener();

	static void openConsole();

	static UI_Element_Label * lbl_stopwatch1, * lbl_stopwatch2, * lbl_stopwatch3, * lbl_stopwatch4;
	static StopWatch stopwatch1, stopwatch2, stopwatch3, stopwatch4;
};