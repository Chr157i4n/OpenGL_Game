#pragma once
#include "defines.h"

#include <vector>
#include <memory>
#include <string>
#include "libs/glm/glm.hpp"



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

	static glm::vec2 getMapSize()
	{
		return mapSize;
	}

	static void setMapSize(glm::vec2 mapSize)
	{
		Map::mapSize = mapSize;
	}

private:

	static std::string mapFileName;
	static std::string mapFileFolder;
	static std::string mapFileExtension;

	static glm::vec3 gravity;

	static glm::vec2 mapSize;
};

