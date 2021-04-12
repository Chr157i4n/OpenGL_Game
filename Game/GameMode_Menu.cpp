#include "GameMode_Menu.h"

#include "Logger.h"
#include "Game.h"

void GameMode_Menu::init()
{
}

void GameMode_Menu::deinit()
{
}

void GameMode_Menu::gameLoop()
{

	stopwatch1.start();
#ifdef DEBUG_LOOP
	Logger::log("Debug Input Step");
#endif
	Game::processInput();
	double stopwatch1duration = stopwatch1.stop();
	Game::lbl_stopwatch1->setText("Input: " + std::to_string(stopwatch1duration));

	GLCALL(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
	GLCALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));


	stopwatch1.start();
#ifdef DEBUG_LOOP
	Logger::log("Debug Render Step");
#endif
	if (Game::gameState != GameState::GAME_PAUSED)
	{
		Game::render();
	}
	double stopwatch4duration = stopwatch1.stop();
	Game::lbl_stopwatch4->setText("Render: " + std::to_string(stopwatch4duration));

}
