#include "Map.h"

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

	Map::mapSize.x = Game::map[0]->getDimensions().x;
	Map::mapSize.y = Game::map[0]->getDimensions().z;
}

void Map::restart()
{	

	Game::objects.clear();
	Game::characters.clear();
	Game::npcs.clear();

	ResourceManager::loadMap(mapFileName);

	for (std::shared_ptr<Object> object : Game::objects)
	{
		object->setHealth(100);
	}


	Game::setGameState(GameState::GAME_ACTIVE);
}
