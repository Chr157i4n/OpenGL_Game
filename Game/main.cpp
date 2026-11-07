#include "Platform.h"
#include "Game.h"

#ifdef _WIN32
#include <winsock2.h>
#include <windows.h>
#endif

int main(int argc, char* argv[])
{
	std::srand(std::time(nullptr));

	Platform::setWorkingDirectoryToExecutable();

	ConfigManager::init("config.ini");

#ifdef _WIN32
	AllocConsole();
	HWND console = FindWindowA("ConsoleWindowClass", NULL);
	if (console)
	{
		ShowWindow(console, ConfigManager::show_debug_console ? SW_SHOW : SW_HIDE);
	}
#endif

	Game::init();

	return 0;
}
