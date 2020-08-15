#include "ResourceManager.h"

#include "Logger.h"
#include "NPC.h"
#include "ConfigManager.h"
#include "Object.h"
#include "Game.h"

std::string ResourceManager::modelFolder = "models";
float ResourceManager::percentageLoading = 0;

std::vector<Model*> ResourceManager::models;




std::vector<Model*> ResourceManager::loadAllModels(std::string modelFileName)
{
	Logger::log("Loading all Models");

	std::string newmodefilename;
	std::vector<Model*> models;
	int id = 0;
	std::list<std::string> modelfilenames;

	tinyxml2::XMLDocument doc;
	doc.LoadFile(modelFileName.c_str());


	//get all modelnames
	modelfilenames.push_back("replacemeball.bmf");

	for (tinyxml2::XMLElement* xmlNodeObject = doc.FirstChildElement("map")->FirstChildElement("models")->FirstChildElement("model"); xmlNodeObject != NULL; xmlNodeObject = xmlNodeObject->NextSiblingElement())
	{
		newmodefilename = xmlNodeObject->GetText();
		bool alreadyLoaded = false;

		for (std::string modefilename : modelfilenames)
		{
			if (modefilename == newmodefilename)
			{
				alreadyLoaded = true;
				break;
			}
		}
		if (!alreadyLoaded)
		{
			modelfilenames.push_back(newmodefilename);
		}
	}

	for (tinyxml2::XMLElement* xmlNodeObject = doc.FirstChildElement("map")->FirstChildElement("objects")->FirstChildElement("object"); xmlNodeObject != NULL; xmlNodeObject = xmlNodeObject->NextSiblingElement())
	{
		newmodefilename = xmlNodeObject->FirstChildElement("modelfile")->GetText();
		bool alreadyLoaded = false;

		for (std::string modefilename : modelfilenames)
		{
			if (modefilename == newmodefilename)
			{
				alreadyLoaded = true;
				break;
			}
		}
		if (!alreadyLoaded)
		{
			modelfilenames.push_back(newmodefilename);
		}
	}

	for (tinyxml2::XMLElement* xmlNodeObject = doc.FirstChildElement("map")->FirstChildElement("bots")->FirstChildElement("bot"); xmlNodeObject != NULL; xmlNodeObject = xmlNodeObject->NextSiblingElement())
	{
		newmodefilename = xmlNodeObject->FirstChildElement("modelfile")->GetText();
		bool alreadyLoaded = false;

		for (std::string modefilename : modelfilenames)
		{
			if (modefilename == newmodefilename)
			{
				alreadyLoaded = true;
				break;
			}
		}
		if (!alreadyLoaded)
		{
			modelfilenames.push_back(newmodefilename);
		}
	}

	//load models
	float modelCount = modelfilenames.size();

	for (std::string modefilename : modelfilenames)
	{

		Model* newModel = loadModel(modelFolder + "/" + modefilename);
		if (newModel == nullptr) continue;

		newModel->setModelName(modefilename);
		newModel->setModelID(id++);

		percentageLoading += 80 / modelCount;
		Logger::log("loaded Model " + std::to_string(newModel->getModelID()) + ": " + newModel->getModelName() + " - " + Helper::to_string_with_precision(percentageLoading, 0) + "%");

		models.push_back(newModel);
	}

	ResourceManager::models=models;

	Logger::log("Loading all Models - finished");
	return models;
}

Model* ResourceManager::loadModel(std::string modelFileName)
{
	Model* newModel = new Model();

	const char* fileNameChar = modelFileName.c_str();
	if (!newModel->init(fileNameChar))
	{
		return nullptr;
	}

	return newModel;
}

Model* ResourceManager::getModelByName(std::string modelFileName)
{
	for (Model* model : models)
	{
		if (model->getModelName() == modelFileName)
		{
			return model;
		}
	}
	return models[0];
}


std::vector<std::string> ResourceManager::readAllMaps()
{
	std::vector<std::string> maps;
	std::string path = "levels";
	if (!std::filesystem::exists(path)) return maps;

	for (const auto& entry : std::filesystem::directory_iterator(path))
	{
		maps.push_back(entry.path().string());
	}

	return maps;
}

void ResourceManager::loadMap(std::string mapFileName)
{
	Game::map.clear();
	Game::objects.clear();
	Game::characters.clear();
	Game::npcs.clear();
	Game::bullets.clear();

	std::vector<std::shared_ptr<Object>>* map = &Game::map;
	std::vector<std::shared_ptr<Object>>* objects = &Game::objects;
	std::vector<std::shared_ptr<Character>>* characters = &Game::characters;
	std::vector< std::shared_ptr<NPC>>* npcs = &Game::npcs;

	const char* mapFileNameC = mapFileName.c_str();
	int32 numObject = 0;
	tinyxml2::XMLDocument doc;
	std::string xmlNodeText;
	std::vector<std::string> params;

	Logger::log("Loading Map");
	doc.LoadFile(mapFileNameC);

	std::string title = doc.FirstChildElement("map")->FirstChildElement("name")->GetText();



	Logger::log("Loading all Objects");
	float objectCount = 0;
	for (tinyxml2::XMLElement* xmlNodeObject = doc.FirstChildElement("map")->FirstChildElement("objects")->FirstChildElement("object"); xmlNodeObject != NULL; xmlNodeObject = xmlNodeObject->NextSiblingElement())
	{
		objectCount++;
	}

	for (tinyxml2::XMLElement* xmlNodeObject = doc.FirstChildElement("map")->FirstChildElement("objects")->FirstChildElement("object"); xmlNodeObject != NULL; xmlNodeObject = xmlNodeObject->NextSiblingElement())
	{
		xmlNodeText = xmlNodeObject->FirstChildElement("modelfile")->GetText();

		std::shared_ptr<Object> newObject = std::make_shared<Object>(xmlNodeText);

		xmlNodeText = xmlNodeObject->FirstChildElement("position")->GetText();
		Helper::split(xmlNodeText, params, ';');
		newObject->setPosition(glm::vec3(stof(params[0]), stof(params[1]), stof(params[2])));

		xmlNodeText = xmlNodeObject->FirstChildElement("rotation")->GetText();
		Helper::split(xmlNodeText, params, ';');
		newObject->setRotation(glm::vec3(stof(params[0]), stof(params[1]), stof(params[2])));

		xmlNodeText = xmlNodeObject->FirstChildElement("scale")->GetText();
		Helper::split(xmlNodeText, params, ';');
		newObject->setScale(glm::vec3(stof(params[0]), stof(params[1]), stof(params[2])));

		xmlNodeText = xmlNodeObject->FirstChildElement("type")->GetText();
		newObject->setType(Object::convertStringToType(xmlNodeText));

		xmlNodeText = xmlNodeObject->FirstChildElement("collisionboxtype")->GetText();
		newObject->setCollisionBoxType(Object::convertStringToCollisionBoxType(xmlNodeText));

		xmlNodeText = xmlNodeObject->FirstChildElement("gravity")->GetText();
		if (xmlNodeText == "true") { newObject->setGravity(true); }
		else { newObject->setGravity(false); }

		xmlNodeText = xmlNodeObject->FirstChildElement("name")->GetText();
		newObject->setName(xmlNodeText);

		newObject->setNumber(numObject);

		percentageLoading += 10 / objectCount;
		Logger::log("loaded Object:" + newObject->printObject() + " - " + Helper::to_string_with_precision(percentageLoading, 0) + "%");
		objects->push_back(newObject);

		if (newObject->getType() == ObjectType::Object_Environment)
		{
			map->push_back(newObject);
		}



		numObject++;
	}
	Logger::log("Loading all Objects - finished");



	Logger::log("Loading all NPCs");
	float npcCount = 0;
	for (tinyxml2::XMLElement* xmlNodeBot = doc.FirstChildElement("map")->FirstChildElement("bots")->FirstChildElement("bot"); xmlNodeBot != NULL; xmlNodeBot = xmlNodeBot->NextSiblingElement())
	{
		npcCount++;
	}

	for (tinyxml2::XMLElement* xmlNodeBot = doc.FirstChildElement("map")->FirstChildElement("bots")->FirstChildElement("bot"); xmlNodeBot != NULL; xmlNodeBot = xmlNodeBot->NextSiblingElement())
	{


		std::shared_ptr<NPC> newNPC = std::make_shared<NPC>();

		xmlNodeText = xmlNodeBot->FirstChildElement("modelfile")->GetText();
		newNPC->setModel(xmlNodeText);

		xmlNodeText = xmlNodeBot->FirstChildElement("position")->GetText();
		Helper::split(xmlNodeText, params, ';');
		newNPC->setPosition(glm::vec3(stof(params[0]), stof(params[1]), stof(params[2])));

		xmlNodeText = xmlNodeBot->FirstChildElement("rotation")->GetText();
		Helper::split(xmlNodeText, params, ';');
		newNPC->setRotation(glm::vec3(stof(params[0]), stof(params[1]), stof(params[2])));

		xmlNodeText = xmlNodeBot->FirstChildElement("scale")->GetText();
		Helper::split(xmlNodeText, params, ';');
		newNPC->setScale(glm::vec3(stof(params[0]), stof(params[1]), stof(params[2])));


		xmlNodeText = xmlNodeBot->FirstChildElement("name")->GetText();
		newNPC->setName(xmlNodeText);

		xmlNodeText = xmlNodeBot->FirstChildElement("team")->GetText();
		newNPC->setTeam(std::stoi(xmlNodeText));

		if (xmlNodeBot->FirstChildElement("navpoints"))
		{
			if (xmlNodeBot->FirstChildElement("navpoints")->FirstChildElement("navpoint"))
			{
				newNPC->setCurrentTask(CurrentTask::Follow_NavPoint);
			}
			else if (newNPC->getTeam() == 0)
			{
				newNPC->setCurrentTask(CurrentTask::Follow_Character);
			}
			else
			{
				newNPC->setCurrentTask(CurrentTask::Attack);
			}


			for (tinyxml2::XMLElement* xmlNodeNavPoint = xmlNodeBot->FirstChildElement("navpoints")->FirstChildElement("navpoint"); xmlNodeNavPoint != NULL; xmlNodeNavPoint = xmlNodeNavPoint->NextSiblingElement())
			{
				xmlNodeText = xmlNodeNavPoint->GetText();
				Helper::split(xmlNodeText, params, ';');
				newNPC->addNavPoint(glm::vec3(stof(params[0]), stof(params[1]), stof(params[2])));
			}
		}


		newNPC->setNumber(numObject);

		percentageLoading += 10 / npcCount;
		Logger::log("loaded NPC:" + newNPC->printObject() + " - " + Helper::to_string_with_precision(percentageLoading, 0) + "%");

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

		std::shared_ptr<NPC> newNpc = std::make_shared<NPC>();
		newNpc->setPosition(glm::vec3(x, 0, z));
		newNpc->setNumber(numObject);
		newNpc->setCurrentTask(CurrentTask::Follow_Character);

		Logger::log("created NPC:" + newNpc->printObject());
		objects->push_back(newNpc);
		npcs->push_back(newNpc);
		characters->push_back(newNpc);
		numObject++;
	}
	Logger::log("Loading all NPCs - finished");

	int id = 100;
	for (std::shared_ptr<Object> object : Game::objects)
	{
		object->setNetworkID(id);
		id++;
	}

	Logger::log("Loading Map - finished");
}
