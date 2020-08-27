#pragma once
#include "defines.h"
#include "Renderer.h"

#include <vector>
#include <memory>
#include <string>

#include <irrKlang.h>


struct PointLight {
	glm::vec3 position = glm::vec3(0,0,0);
	int positionUniformIndex = -1;

	glm::vec3 color = glm::vec3(0, 0, 0);
	int colorUniformIndex = -1;

	float linear = 0.1;
	float quadratic = 0.01;

	bool active = true;
};


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

	static std::vector<PointLight> pointLights;

private:


	static std::string mapFileName;
	static std::string mapFileFolder;
	static std::string mapFileExtension;

	static glm::vec3 gravity;

	static glm::vec2 mapSize;

	static unsigned int skyboxTexture;

	static std::vector<irrklang::ISound*> ambientSounds;
};

