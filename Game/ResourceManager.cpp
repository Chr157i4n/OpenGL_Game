#include "ResourceManager.h"


ResourceManager::ResourceManager(ConfigManager* configManager)
{
	this->configManager = configManager;
}

void ResourceManager::loadShader(std::string vertexShaderFilename, std::string fragmentShaderFilename)
{
	shader = new Shader(vertexShaderFilename, fragmentShaderFilename);
	shader->bind();
}

Shader* ResourceManager::getShader()
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
	std::ifstream mapFile;
	mapFile.open(mapFileName);

	if (!mapFile) {
		std::cout << "Cant open Map: " << mapFileName << std::endl;
		return;
	}

	char line[255];

	while (!mapFile.eof())
	{
		std::vector<std::string> param;
		mapFile.getline(line, 255);

		if (line[0] == '\0' || line[0] == '#')
		{
			continue;
		}

		split(line, param, ',');

		std::string fileName = "models/" + param[0];

		Object* newObject = new Object(shader, fileName);
		newObject->setPosition(glm::vec3(stof(param[1]), stof(param[2]), stof(param[3])));
		newObject->setRotation(glm::vec3(stof(param[4]), stof(param[5]), stof(param[6])));
		newObject->setObjectType(ObjectType::Object_Entity);
		newObject->calculateDimensions();
		objects->push_back(newObject);
	}

	(*objects)[0]->setObjectType(ObjectType::Object_Environment);

	Player* player = new Player(shader, 90.0f, 800.0f, 600.0f);
	objects->push_back(player);
	players->push_back(player);

	int botcount = std::stoi(configManager->readConfig("bots"));
	for (int i = 0; i < botcount; i++)
	{
		float x = rand() % 100 - 50;
		float z = rand() % 100 - 50;

		NPC* npc1 = new NPC(shader, 90.0f, 800.0f, 600.0f);
		npc1->setPosition(glm::vec3(x, 0, z));
		objects->push_back(npc1);
		npcs->push_back(npc1);
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