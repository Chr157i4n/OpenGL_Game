#pragma once
#include "defines.h"

#include <vector>
#include <memory>
#include <string>



class Map
{

public:
	static void load(std::string mapFileName);

	static void restart();
private:

	static std::string mapFileName;
	static std::string mapFileFolder;
};

