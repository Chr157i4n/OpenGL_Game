#pragma once
#include "defines.h"

#include <vector>
#include <memory>
#include <string>

#include <irrKlang.h>



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

	static std::vector<std::string> getSkyboxFaces();

	static void loadSkybox();

	static unsigned int getSkyboxTexture()
	{
		return skyboxTexture;
	}

	static std::string mapName;
	static glm::vec3 sun_direction;
	static glm::vec3 sun_color;
	static std::string skyboxName;

private:


	static std::string mapFileName;
	static std::string mapFileFolder;
	static std::string mapFileExtension;

	static glm::vec3 gravity;

	static glm::vec2 mapSize;

	static unsigned int skyboxTexture;

	static std::vector<irrklang::ISound*> ambientSounds;
};

