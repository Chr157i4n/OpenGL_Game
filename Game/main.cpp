#include "Platform.h"
#include "Game.h"
#include "Localization.h"

#ifdef _WIN32
#include <winsock2.h>
#include <windows.h>

static LONG CALLBACK ogCrashHandler(EXCEPTION_POINTERS* info)
{
	char buffer[8192];
	int written = 0;
	uintptr_t base = reinterpret_cast<uintptr_t>(GetModuleHandleA(nullptr));
	void* frames[48];
	USHORT count = CaptureStackBackTrace(0, 48, frames, nullptr);

	written += sprintf_s(buffer + written, sizeof(buffer) - written,
		"CRASH code=0x%08lX addr=0x%p offset=0x%p base=0x%p frames=%u\n",
		static_cast<unsigned long>(info->ExceptionRecord->ExceptionCode),
		info->ExceptionRecord->ExceptionAddress,
		reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(info->ExceptionRecord->ExceptionAddress) - base),
		reinterpret_cast<void*>(base),
		static_cast<unsigned int>(count));

	for (USHORT i = 0; i < count && written < static_cast<int>(sizeof(buffer)) - 96; ++i)
	{
		uintptr_t frame = reinterpret_cast<uintptr_t>(frames[i]);
		written += sprintf_s(buffer + written, sizeof(buffer) - written,
			"  #%u 0x%p offset=0x%p\n", static_cast<unsigned int>(i), frames[i],
			reinterpret_cast<void*>(frame - base));
	}

	HANDLE file = CreateFileA("crash_stack.txt", GENERIC_WRITE, FILE_SHARE_READ, nullptr,
		CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (file != INVALID_HANDLE_VALUE)
	{
		DWORD bytesWritten = 0;
		WriteFile(file, buffer, static_cast<DWORD>(written), &bytesWritten, nullptr);
		CloseHandle(file);
	}

	return EXCEPTION_CONTINUE_SEARCH;
}
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

	ConfigManager::init("config.json");
	Locale::init(ConfigManager::language);

	Game::startupMap = getMapCommandLineArgument(argc, argv);

#ifdef _WIN32
	AddVectoredExceptionHandler(1, ogCrashHandler);
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
