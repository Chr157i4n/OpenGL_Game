#include "ResourceManager.h"
#include "Logger.h"


Shader* ResourceManager::shader;


void ResourceManager::loadShader(std::string vertexShaderFilename, std::string fragmentShaderFilename)
{
	shader = new Shader(vertexShaderFilename, fragmentShaderFilename);
	shader->bind();
}

Shader* ResourceManager::getObjectShader()
{
	return shader;
}

void ResourceManager::bindShader()
{
	shader->bind();
}

void ResourceManager::unbindShader()
{
	shader->unbind();
}

void ResourceManager::loadMap(std::string mapFileName, std::vector<Object*>* objects, std::vector<Player*>* players, std::vector<NPC*>* npcs)
{
	int32 numObject = 0;
	char mapFileLine[255];

	std::ifstream mapFile;
	mapFile.open(mapFileName);

	if (!mapFile) {
		Logger::log("Cant open Map: " + mapFileName);

		return;
	}	

	//Player(s)
	float fov = std::stof(ConfigManager::readConfig("fov"));
	Player* player = new Player(shader, fov, 800.0f, 600.0f);
	player->setNumber(numObject);
	objects->push_back(player);
	players->push_back(player);
	numObject++;
	
	//Enviroment and Entities
	while (!mapFile.eof())
	{
		std::vector<std::string> mapParams;
		mapFile.getline(mapFileLine, 255);

		if (mapFileLine[0] == '\0' || mapFileLine[0] == '#' || (mapFileLine[0] == '/' && mapFileLine[1] == '/'))
		{
			continue;
		}

		split(mapFileLine, mapParams, ',');

		std::string fileName = "models/" + mapParams[0];

		Object* newObject = new Object(shader, fileName);
		newObject->setPosition(glm::vec3(stof(mapParams[1]), stof(mapParams[2]), stof(mapParams[3])));
		newObject->setRotation(glm::vec3(stof(mapParams[4]), stof(mapParams[5]), stof(mapParams[6])));
		newObject->setType(ObjectType::Object_Entity);
		newObject->setNumber(numObject);
		objects->push_back(newObject);

		numObject++;
	}
	(*objects)[1]->setType(ObjectType::Object_Environment);

	//Bots
	int botcount = std::stoi(ConfigManager::readConfig("bots"));
	for (int i = 0; i < botcount; i++)
	{
		float x = rand() % 100 - 50;
		float z = rand() % 100 - 50;

		NPC* npc = new NPC(shader, 90.0f, 800.0f, 600.0f);
		npc->setPosition(glm::vec3(x, 0, z));
		npc->setNumber(numObject);
		objects->push_back(npc);
		npcs->push_back(npc);
		numObject++;
	}
}

size_t ResourceManager::split(const std::string& txt, std::vector<std::string>& strs, char ch)
{
	size_t pos = txt.find(ch);
	size_t initialPos = 0;
	strs.clear();

	// Decompose statement
	while (pos != std::string::npos) {
		strs.push_back(txt.substr(initialPos, pos - initialPos));
		initialPos = pos + 1;

		pos = txt.find(ch, initialPos);
	}

	// Add the last one
	strs.push_back(txt.substr(initialPos, std::min(pos, txt.size()) - initialPos + 1));

	return strs.size();
}