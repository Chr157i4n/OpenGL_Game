#pragma once
#include "GameMode.h"

#include "StopWatch.h"

class GameMode_Menu : public GameMode
{
public:
	virtual void init();

	virtual void deinit();

	virtual void gameLoop();

private:

};