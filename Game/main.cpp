#include "Platform.h"
#include "Game.h"

#ifdef _WIN32
#include <winsock2.h>
#include <windows.h>
#endif

static std::string getMapCommandLineArgument(int argc, char* argv[])
{
	for (int i = 1; i < argc; i++)
	{
		std::string arg = argv[i];

	 if (arg == "--map" || arg == "-map" || arg == "-m" || arg == "/map")
		{
			if (i + 1 < argc)
			{
				return std::string(argv[++i]);
			}

			return "";
		}

		if (arg.rfind("--map=", 0) == 0)
		{
			return arg.substr(6);
		}

		if (arg.rfind("-m=", 0) == 0)
		{
			return arg.substr(3);
		}

		if (i == 1 && arg[0] != '-')
		{
			return arg;
		}
	}

	return "";
}

int main(int argc, char* argv[])
{
	std::srand(std::time(nullptr));

	Platform::setWorkingDirectoryToExecutable();

	ConfigManager::init("config.ini");

	Game::startupMap = getMapCommandLineArgument(argc, argv);

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
