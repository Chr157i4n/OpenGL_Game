#include "ResourceManager.h"
#include "Logger.h"



Shader* ResourceManager::shader;
std::string ResourceManager::modelFolder = "models";

void ResourceManager::init()
{

}

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

void ResourceManager::loadMap(std::string mapFileName, std::vector<Object*>* objects, std::vector<Character*>* characters, std::vector<Player*>* players, std::vector<NPC*>* npcs)
{
	const char* mapFileNameC = mapFileName.c_str();
	int32 numObject = 0;
	tinyxml2::XMLDocument doc;
	std::string xmlNodeText;
	std::vector<std::string> params;



	doc.LoadFile(mapFileNameC);

	std::string title = doc.FirstChildElement("map")->FirstChildElement("name")->GetText();



	//Player(s)
	float fov = std::stof(ConfigManager::readConfig("fov"));
	Player* player = new Player(shader, fov, 800.0f, 600.0f);
	player->setName("Player");
	player->setNumber(numObject);
	objects->push_back(player);
	players->push_back(player);
	characters->push_back(player);
	numObject++;




	for (tinyxml2::XMLElement* xmlNodeObject = doc.FirstChildElement("map")->FirstChildElement("objects")->FirstChildElement("object"); xmlNodeObject != NULL; xmlNodeObject = xmlNodeObject->NextSiblingElement())
	{
		xmlNodeText = xmlNodeObject->FirstChildElement("modelfile")->GetText();
		xmlNodeText = modelFolder + "/" + xmlNodeText;

		Object* newObject = new Object(shader, xmlNodeText);

		xmlNodeText = xmlNodeObject->FirstChildElement("position")->GetText();
		split(xmlNodeText, params, ';');
		newObject->setPosition(glm::vec3(stof(params[0]), stof(params[1]), stof(params[2])));

		xmlNodeText = xmlNodeObject->FirstChildElement("rotation")->GetText();
		split(xmlNodeText, params, ';');
		newObject->setRotation(glm::vec3(stof(params[0]), stof(params[1]), stof(params[2])));

		xmlNodeText = xmlNodeObject->FirstChildElement("scale")->GetText();
		split(xmlNodeText, params, ';');
		newObject->setScale(glm::vec3(stof(params[0]), stof(params[1]), stof(params[2])));

		xmlNodeText = xmlNodeObject->FirstChildElement("type")->GetText();
		newObject->setType(Object::convertStringToType(xmlNodeText));

		xmlNodeText = xmlNodeObject->FirstChildElement("name")->GetText();
		newObject->setName(xmlNodeText);

		newObject->setNumber(numObject);
		objects->push_back(newObject);

		numObject++;
	}



	for (tinyxml2::XMLElement* xmlNodeBot = doc.FirstChildElement("map")->FirstChildElement("bots")->FirstChildElement("bot"); xmlNodeBot != NULL; xmlNodeBot = xmlNodeBot->NextSiblingElement())
	{
		xmlNodeText = xmlNodeBot->FirstChildElement("modelfile")->GetText();
		xmlNodeText = modelFolder + "/" + xmlNodeText;

		NPC* newNPC = new NPC(shader);

		xmlNodeText = xmlNodeBot->FirstChildElement("position")->GetText();
		split(xmlNodeText, params, ';');
		newNPC->setPosition(glm::vec3(stof(params[0]), stof(params[1]), stof(params[2])));

		xmlNodeText = xmlNodeBot->FirstChildElement("rotation")->GetText();
		split(xmlNodeText, params, ';');
		newNPC->setRotation(glm::vec3(stof(params[0]), stof(params[1]), stof(params[2])));

		xmlNodeText = xmlNodeBot->FirstChildElement("scale")->GetText();
		split(xmlNodeText, params, ';');
		newNPC->setScale(glm::vec3(stof(params[0]), stof(params[1]), stof(params[2])));

		xmlNodeText = xmlNodeBot->FirstChildElement("type")->GetText();
		newNPC->setType(Object::convertStringToType(xmlNodeText));

		xmlNodeText = xmlNodeBot->FirstChildElement("name")->GetText();
		newNPC->setName(xmlNodeText);

		if (xmlNodeBot->FirstChildElement("navpoints"))
		{
			if (xmlNodeBot->FirstChildElement("navpoints")->FirstChildElement("navpoint"))
			{
				newNPC->setCurrentTask(CurrentTask::Follow_NavPoint);
			}
			else
			{
				newNPC->setCurrentTask(CurrentTask::Follow_Character);
			}

			
			for (tinyxml2::XMLElement* xmlNodeNavPoint = xmlNodeBot->FirstChildElement("navpoints")->FirstChildElement("navpoint"); xmlNodeNavPoint != NULL; xmlNodeNavPoint = xmlNodeNavPoint->NextSiblingElement())
			{
				xmlNodeText = xmlNodeNavPoint->GetText();
				split(xmlNodeText, params, ';');
				newNPC->addNavPoint(glm::vec3(stof(params[0]), stof(params[1]), stof(params[2])));
			}
		}


		newNPC->setNumber(numObject);
		objects->push_back(newNPC);
		npcs->push_back(newNPC);
		characters->push_back(newNPC);

		numObject++;
	}



	//random bots
	int botcount = std::stoi(ConfigManager::readConfig("bots"));
	for (int i = 0; i < botcount; i++)
	{
		float x = rand() % 100 - 50;
		float z = rand() % 100 - 50;

		NPC* newNpc = new NPC(shader);
		newNpc->setPosition(glm::vec3(x, 0, z));
		newNpc->setNumber(numObject);
		newNpc->setCurrentTask(CurrentTask::Follow_Character);
		objects->push_back(newNpc);
		npcs->push_back(newNpc);
		characters->push_back(newNpc);
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