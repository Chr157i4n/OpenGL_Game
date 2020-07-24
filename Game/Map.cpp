#include "Map.h"

#include "UI.h"
#include "ResourceManager.h"
#include "Game.h"

std::string Map::mapFileName = "";
std::string Map::mapFileFolder = "levels/";

void Map::load(std::string mapFileName)
{
	Map::mapFileName = mapFileName;
	
	ResourceManager::loadMap(mapFileFolder+mapFileName);
}

void Map::restart()
{	
	Game::objects.clear();
	Game::characters.clear();
	Game::players.clear();
	Game::npcs.clear();

	ResourceManager::reloadMap(mapFileFolder+mapFileName);

	for (std::shared_ptr<Object> object : Game::objects)
	{
		object->setHealth(100);
	}

	UI::clearUI_Elements();

	Game::players[0]->createHealthbar();
}
