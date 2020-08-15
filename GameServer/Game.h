#pragma once
#include "defines.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h> 
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <memory>
#include <chrono>
#include <thread>


#include "libs/glm/glm.hpp"
#include "libs/glm/ext/matrix_transform.hpp"
#include "libs/glm/gtc/matrix_transform.hpp"


//Game Logic
#include "Object.h"
#include "NPC.h"
#include "Bullet.h"
#include "Character.h"
#include "Client.h"

//Resourcemanagment
#include "ResourceManager.h"
#include "ConfigManager.h"
#include "Map.h"


//Utilitiy
#include "StopWatch.h"
//#include "LuaManager.h"
#include "NetworkManager.h"
#include "Helper.h"

//#define DEBUG_COLLISION
//#define DEBUG_OUTOFBOUNDS
//#define DEBUG_GRAVITY
//#define DEBUG_NPC
//#define DEBUG_ENV_MAP

enum GameState {
	GAME_LOADING,
	GAME_ACTIVE,
	GAME_GAME_OVER,
	GAME_PAUSED,
	GAME_MENU,
};

enum GameStateOnline {
	Game_Online,
	Game_Offline,
};


enum MenuType {
	MENU_MAIN,
	MENU_OPTIONS,
	MENU_PAUSE,
};

static class Game
{
public:
	static void init();

	static void startGame();

	static void initMultiplayer();

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

	static void setGameState(GameState newGameState)
	{
		gameState = newGameState;
	}

	static GameState getGameState()
	{
		return gameState;
	}

	static void quit()
	{
		ConfigManager::writeAllConfigs();
		close = true;
	}

	static float64 getTimeStamp()
	{
		return gameStopWatch.stop();	
	}

	static void sleep(int ms);


	static std::vector< std::shared_ptr<Object> > map;
	static std::vector< std::shared_ptr<Object> > objects;
	static std::vector< std::shared_ptr<Character> > characters;
	static std::vector< std::shared_ptr<NPC> > npcs;
	static std::vector< std::shared_ptr<Bullet> > bullets;

	static std::vector<std::shared_ptr<Client>> clients;

	static bool close;


	static float32 delta;

	static GameState gameState;
	static GameStateOnline gameStateOnline;


private:

	static void processCollision();

	static void render();

	static void gameLoop();

	static void deleteObjects();

	static StopWatch stopwatch1;
	static StopWatch gameStopWatch;
	static StopWatch frametimeStopWatch;
};