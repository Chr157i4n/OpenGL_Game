#include "Map.h"

std::string Map::mapFileName = "";
std::string Map::mapFileFolder = "levels/";

void Map::load(std::string mapFileName, std::vector<std::shared_ptr<Object>>* objects, std::vector< std::shared_ptr<Character>>* characters, std::vector< std::shared_ptr<Player>>* players, std::vector< std::shared_ptr<NPC>>* npcs)
{
	Map::mapFileName = mapFileName;
	
	ResourceManager::loadMap(mapFileFolder+mapFileName, objects, characters, players, npcs);
}

void Map::restart(std::vector<std::shared_ptr<Object>>* objects, std::vector< std::shared_ptr<Character>>* characters, std::vector< std::shared_ptr<Player>>* players, std::vector< std::shared_ptr<NPC>>* npcs)
{	
	objects->clear();
	characters->clear();
	players->clear();
	npcs->clear();

	ResourceManager::reloadMap(mapFileFolder+mapFileName, objects, characters, players, npcs);
}
