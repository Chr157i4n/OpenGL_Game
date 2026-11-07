#include "StopWatch.h"

void StopWatch::start()
{
	startTimeStamp = std::chrono::system_clock::now();

}

float64 StopWatch::stop()
{
	std::chrono::system_clock::time_point stopTimeStamp = std::chrono::system_clock::now();

	std::chrono::duration<double, std::milli> duration = stopTimeStamp - startTimeStamp;

	return duration.count();
}
