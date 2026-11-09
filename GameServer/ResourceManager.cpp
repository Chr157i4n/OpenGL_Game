#include "ResourceManager.h"

#include <list>

#include "Logger.h"
#include "NPC.h"
#include "ConfigManager.h"
#include "Object.h"
#include "Game.h"
#include "Map.h"

std::string ResourceManager::modelFolder = "models";
float ResourceManager::percentageLoading = 0;

std::vector<Model *> ResourceManager::models;

static glm::vec3 jsonVec3(const nlohmann::json& v)
{
	return glm::vec3(v[0].get<float>(), v[1].get<float>(), v[2].get<float>());
}

static nlohmann::json parseMapFile(const std::string& mapFileName)
{
	std::ifstream file(mapFileName);
	if (!file)
	{
		Logger::log("Could not open map file: " + mapFileName);
		return nlohmann::json();
	}

	try
	{
		return nlohmann::json::parse(file);
	}
	catch (const std::exception& e)
	{
		Logger::log("Could not parse map file " + mapFileName + ": " + e.what());
		return nlohmann::json();
	}
}

std::vector<Model *> ResourceManager::loadAllModels(std::string modelFileName)
{
	Logger::log("Loading all Models");

	std::vector<Model *> models;
	int id = 0;
	std::list<std::string> modelfilenames;

	nlohmann::json doc = parseMapFile(modelFileName);

	// the models which are not defined by an object but by the level itself
	if (doc.contains("default_models"))
		loadDefaultModels(doc["default_models"]);

	// get all modelnames
	modelfilenames.push_back(Map::fallbackModelName);
	modelfilenames.push_back(Map::characterModelName);
	modelfilenames.push_back(Map::bulletModelName);

	auto addModel = [&](const std::string& name)
	{
		for (const std::string& existing : modelfilenames)
		{
			if (existing == name)
				return;
		}
		modelfilenames.push_back(name);
	};

	if (doc.contains("models"))
		for (const auto& model : doc["models"])
		addModel(model.get<std::string>());

	if (doc.contains("objects"))
		for (const auto& object : doc["objects"])
		if (object.contains("modelfile"))
			addModel(object["modelfile"].get<std::string>());

	if (doc.contains("bots"))
		for (const auto& bot : doc["bots"])
		if (bot.contains("modelfile"))
			addModel(bot["modelfile"].get<std::string>());

	// load models
	float modelCount = modelfilenames.size();

	for (std::string modefilename : modelfilenames)
	{

		Model *newModel = loadModel(modelFolder + "/" + modefilename);
		if (newModel == nullptr)
			continue;

		newModel->setModelName(modefilename);
		newModel->setModelID(id++);

		percentageLoading += 80 / modelCount;
		Logger::log("loaded Model " + std::to_string(newModel->getModelID()) + ": " + newModel->getModelName() + " - " + Helper::to_string_with_precision(percentageLoading, 0) + "%");

		models.push_back(newModel);
	}

	ResourceManager::models = models;

	Logger::log("Loading all Models - finished");
	return models;
}

Model *ResourceManager::loadModel(std::string modelFileName)
{
	Model *newModel = new Model();

	const char *fileNameChar = modelFileName.c_str();
	if (!newModel->init(fileNameChar))
	{
		return nullptr;
	}

	return newModel;
}

void ResourceManager::loadDefaultModels(const nlohmann::json& node)
{
	if (node.contains("character_model"))
		Map::characterModelName = node["character_model"].get<std::string>();

	if (node.contains("bullet_model"))
		Map::bulletModelName = node["bullet_model"].get<std::string>();

	if (node.contains("fallback_model"))
		Map::fallbackModelName = node["fallback_model"].get<std::string>();

	Logger::log("Level default models: character: " + Map::characterModelName + ", bullet: " + Map::bulletModelName + ", fallback: " + Map::fallbackModelName);
}

Model *ResourceManager::getModelByName(std::string modelFileName)
{
	for (Model *model : models)
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
	if (!std::filesystem::exists(path))
		return maps;

	for (const auto &entry : std::filesystem::directory_iterator(path))
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

	std::vector<std::shared_ptr<Object>> *map = &Game::map;
	std::vector<std::shared_ptr<Object>> *objects = &Game::objects;
	std::vector<std::shared_ptr<Character>> *characters = &Game::characters;
	std::vector<std::shared_ptr<NPC>> *npcs = &Game::npcs;

	int32 numObject = 0;

	Logger::log("Loading Map");

	nlohmann::json doc = parseMapFile(mapFileName);

	Logger::log("Loading all Objects");
	float objectCount = 0;
	if (doc.contains("objects"))
		objectCount = static_cast<float>(doc["objects"].size());

	if (doc.contains("objects"))
		for (const auto& node : doc["objects"])
		{
			std::shared_ptr<Object> newObject = std::make_shared<Object>(node["modelfile"].get<std::string>());

			newObject->setPosition(jsonVec3(node["position"]));
			newObject->setRotation(jsonVec3(node["rotation"]));
			newObject->setScale(jsonVec3(node["scale"]));

			newObject->setType(Object::convertStringToType(node["type"].get<std::string>()));
			newObject->setCollisionBoxType(Object::convertStringToCollisionBoxType(node["collisionboxtype"].get<std::string>()));
			newObject->setGravity(node.value("gravity", false));

			newObject->setName(node["name"].get<std::string>());

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
	if (doc.contains("bots"))
		npcCount = static_cast<float>(doc["bots"].size());

	if (doc.contains("bots"))
		for (const auto& node : doc["bots"])
		{
			std::shared_ptr<NPC> newNPC = std::make_shared<NPC>();

			newNPC->setModel(node["modelfile"].get<std::string>());
			newNPC->setPosition(jsonVec3(node["position"]));
			newNPC->setRotation(jsonVec3(node["rotation"]));
			newNPC->setScale(jsonVec3(node["scale"]));
			newNPC->setName(node["name"].get<std::string>());
			newNPC->setTeam(node["team"].get<int>());

			if (node.contains("navpoints"))
			{
				if (!node["navpoints"].empty())
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

				for (const auto& navPoint : node["navpoints"])
				{
					newNPC->addNavPoint(jsonVec3(navPoint));
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

	// random bots
	int botcount = ConfigManager::bots;
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
