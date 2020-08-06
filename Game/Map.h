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

	static glm::vec3 getGravity()
	{
		return gravity;
	}

	static void setGravity(glm::vec3 gravity)
	{
		Map::gravity = gravity;
	}

private:

	static std::string mapFileName;
	static std::string mapFileFolder;
	static glm::vec3 gravity;
};

