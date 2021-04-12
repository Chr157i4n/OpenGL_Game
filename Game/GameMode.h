#pragma once

#include "StopWatch.h"

class GameMode
{
public:
	virtual void init();

	virtual void deinit();

	virtual void gameLoop();

protected:
	StopWatch stopwatch1;

};