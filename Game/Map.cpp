#include "Map.h"

#include "UI.h"
#include "ResourceManager.h"
#include "Game.h"

std::string Map::mapFileName = "";
std::string Map::mapFileFolder = "levels\\";
std::string Map::mapFileExtension = ".xml";

glm::vec3 Map::gravity = glm::vec3(0, -80, 0);

glm::vec2 Map::mapSize = glm::vec2(110,110);

void Map::load(std::string mapFileName)
{
	if (mapFileName.find(mapFileFolder) == std::string::npos) {
		mapFileName = mapFileFolder+mapFileName;
	}
	if (mapFileName.find(mapFileExtension) == std::string::npos) {
		mapFileName = mapFileName+mapFileExtension;
	}
	
	Map::mapFileName = mapFileName;
	
	ResourceManager::loadAllModels(mapFileName);
	ResourceManager::loadMap(mapFileName);
	Renderer::loadingProgressBar->setLifespan(-1);

	Map::mapSize.x = Game::map[0]->getDimensions().x;
	Map::mapSize.y = Game::map[0]->getDimensions().z;
}

void Map::restart()
{	
	std::vector<unsigned int> envCubeMaps, envCubeMapFrameBuffers;

	for (std::shared_ptr<Object> object : Game::objects)
	{
		envCubeMaps.push_back(object->getEnvCubeMap());
		envCubeMapFrameBuffers.push_back(object->getEnvCubeMapFrameBuffer());
	}


	Game::objects.clear();
	Game::characters.clear();
	Game::players.clear();
	Game::npcs.clear();

	ResourceManager::loadMap(mapFileName);

	for (std::shared_ptr<Object> object : Game::objects)
	{
		object->setHealth(100);
	}

	UI::clearUI_Elements();

	Game::players[0]->createHealthbar();

	//Renderer::resetFrameCount();
	//todo: if object dies, this doesnt work anymore. perhaps died objects should only be "disabled" instead of deleted
	for (int i = 0; i < Game::objects.size(); i++)
	{
		if(i < envCubeMaps.size())
			Game::objects[i]->setEnvCubeMap(envCubeMaps[i]);
		if (i < envCubeMapFrameBuffers.size())
			Game::objects[i]->setEnvCubeMapFrameBuffer(envCubeMapFrameBuffers[i]);
	}

	Game::setGameState(GameState::GAME_ACTIVE);
	SDL_SetRelativeMouseMode(SDL_TRUE);
}
