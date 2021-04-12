#pragma once
#include "GameMode.h"

class GameMode_MultiPlayer : public GameMode
{
public:
	virtual void init();

	virtual void deinit();

	virtual void gameLoop();

private:

};

