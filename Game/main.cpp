#pragma comment(lib, "SDL2.lib")
#pragma comment(lib, "glew32s.lib")
#pragma comment(lib, "opengl32.lib")


#include "windows.h"
#include "Game.h"




int main(int argc, char* argv[])
{
	srand(time(NULL));

	ConfigManager::init("config.ini");

	HWND console;
	AllocConsole();
	console = FindWindowA("ConsoleWindowClass", NULL);

	if (ConfigManager::readConfig("showdebug_console") == "1")
	{
		ShowWindow(console, 1);
	}
	else
	{
		ShowWindow(console, 0);
	}

	Game::initOpenGL();
	Game::initEverythingElse();

	Game::startGame();


	return 0;
};