#pragma once

#include "defines.h"

#include <chrono>

class StopWatch
{
public:
	void start();

	float64 stop();

private:
	std::chrono::system_clock::time_point startTimeStamp;
};

