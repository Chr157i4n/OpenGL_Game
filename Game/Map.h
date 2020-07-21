#pragma once
#include <vector>
#include <memory>

#include "Object.h"
#include "Character.h"
#include "Player.h"
#include "NPC.h"
#include "Bullet.h"

#include "ResourceManager.h"


class Map
{

public:
	static void load(std::string mapFileName, std::vector<std::shared_ptr<Object>>* objects, std::vector< std::shared_ptr<Character>>* characters, std::vector< std::shared_ptr<Player>>* players, std::vector< std::shared_ptr<NPC>>* npcs);

	static void restart(std::vector<std::shared_ptr<Object>>* objects, std::vector< std::shared_ptr<Character>>* characters, std::vector< std::shared_ptr<Player>>* players, std::vector< std::shared_ptr<NPC>>* npcs);
private:

	static std::string mapFileName;
	static std::string mapFileFolder;
};

