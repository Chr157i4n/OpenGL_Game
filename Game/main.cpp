#include <winsock2.h>
#include "windows.h"
#include "Game.h"




int main(int argc, char* argv[])
{
	
	std::srand(std::time(nullptr));

	ConfigManager::init("config.ini");

	HWND console;
	AllocConsole();
	console = FindWindowA("ConsoleWindowClass", NULL);

	if (ConfigManager::show_debug_console)
	{
		ShowWindow(console, 1);
	}
	else
	{
		ShowWindow(console, 0);
	}

	Game::init();

	//Game::startGame();


	return 0;
};