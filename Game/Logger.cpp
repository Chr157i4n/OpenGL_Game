#include "Logger.h"


void Logger::log(std::string message)
{
	time_t now = time(0);
	tm* ltm = localtime(&now);

	std::string time = "[";

	time += std::to_string(1 + ltm->tm_hour) + ":";
	time += std::to_string(1 + ltm->tm_min) + ":";
	time += std::to_string(1 + ltm->tm_sec) + "] ";

	std::cout << time << message << std::endl;
}