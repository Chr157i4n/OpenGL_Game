#include "Logger.h"


void Logger::log(std::string message)
{
	time_t now = time(0);
	tm* ltm = localtime(&now);

	std::string time = "[";

	if (ltm->tm_hour < 10) time += "0";
	time += std::to_string(ltm->tm_hour) + ":";
	if (ltm->tm_min < 10) time += "0";
	time += std::to_string(ltm->tm_min) + ":";
	if (ltm->tm_sec < 10) time += "0";
	time += std::to_string(ltm->tm_sec) + "] ";

	std::cout << time << message << std::endl;
}