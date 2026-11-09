#include "ResourceManager.h"

#include "Logger.h"
#include "Renderer.h"
#include "Player.h"
#include "NPC.h"
#include "ConfigManager.h"
#include "Object.h"
#include "Game.h"
#include "Button.h"
#include "Map.h"

std::string ResourceManager::modelFolder = "models";
float ResourceManager::percentageLoading = 0;

// Reads a JSON array of numbers into a glm vector.
static glm::vec3 jsonVec3(const nlohmann::json& v)
{
	return glm::vec3(v[0].get<float>(), v[1].get<float>(), v[2].get<float>());
}

static glm::vec2 jsonVec2(const nlohmann::json& v)
{
	return glm::vec2(v[0].get<float>(), v[1].get<float>());
}

static glm::vec4 jsonVec4(const nlohmann::json& v)
{
	return glm::vec4(v[0].get<float>(), v[1].get<float>(), v[2].get<float>(), v[3].get<float>());
}

Shader *ResourceManager::loadShader(std::string vertexShaderFilename, std::string fragmentShaderFilename)
{
	Shader *newShader = new Shader(vertexShaderFilename, fragmentShaderFilename);
	newShader->bind();
	return newShader;
}

std::vector<Model *> ResourceManager::loadAllModels(std::string modelFileName)
{
	Renderer::getModels().clear();
	Logger::log("Loading all Models");

	std::vector<Model *> models;
	int id = 0;
	std::list<std::string> modelfilenames;

	std::ifstream file(modelFileName);
	if (!file)
	{
		Logger::log("Could not open map file: " + modelFileName);
		return models;
	}

	nlohmann::json doc;
	try
	{
		doc = nlohmann::json::parse(file);
	}
	catch (const std::exception& e)
	{
		Logger::log("Could not parse map file " + modelFileName + ": " + e.what());
		return models;
	}

	if (doc.contains("default_models"))
		loadDefaultModels(doc["default_models"]);

	// the models which are not defined by an object but by the level itself
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

	// the player model
	if (doc.contains("player") && doc["player"].contains("modelfile"))
		addModel(doc["player"]["modelfile"].get<std::string>());

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
		Renderer::loadingProgressBar->setValue(percentageLoading);
		Renderer::drawLoadingScreen();

		models.push_back(newModel);
	}

	Renderer::setModels(models);

	Logger::log("Loading all Models - finished");
	return models;
}

Model *ResourceManager::loadModel(std::string modelFileName)
{
	Model *newModel = new Model();

	const char *fileNameChar = modelFileName.c_str();
	if (!newModel->init(fileNameChar, Renderer::getShader(ShaderType::basic)))
	{
		return nullptr;
	}

	return newModel;
}

Model *ResourceManager::getModelByName(std::string modelFileName)
{
	for (Model *model : Renderer::getModels())
	{
		if (model->getModelName() == modelFileName)
		{
			return model;
		}
	}
	return Renderer::getModels()[0];
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
	Game::players.clear();
	Game::npcs.clear();
	Game::bullets.clear();

	Game::texts.clear();
	Map::pointLights.clear();

	int32 numObject = 0;

	Logger::log("Loading Map");

	std::ifstream file(mapFileName);
	if (!file)
	{
		Logger::log("Could not open map file: " + mapFileName);
		return;
	}

	nlohmann::json doc;
	try
	{
		doc = nlohmann::json::parse(file);
	}
	catch (const std::exception& e)
	{
		Logger::log("Could not parse map file " + mapFileName + ": " + e.what());
		return;
	}

	// meta infos
	if (doc.contains("name"))
		Map::mapName = doc["name"].get<std::string>();

	// map properties
	if (doc.contains("map_properties"))
		loadMapProperties(doc["map_properties"]);

	// Player(s)
	loadPlayer(doc);

	Logger::log("Loading all Objects");
	float objectCount = 0;
	if (doc.contains("objects"))
		objectCount = static_cast<float>(doc["objects"].size());

	if (doc.contains("objects"))
		for (const auto& object : doc["objects"])
		loadObject(object, objectCount);
	Logger::log("Loading all Objects - finished");

	Logger::log("Loading all NPCs");
	float npcCount = 0;
	if (doc.contains("bots"))
		npcCount = static_cast<float>(doc["bots"].size());

	if (doc.contains("bots"))
		for (const auto& bot : doc["bots"])
		loadNpc(bot, npcCount);

	// random bots
	for (int i = 0; i < ConfigManager::bots; i++)
	{
		float x = rand() % 100 - 50;
		float z = rand() % 100 - 50;

		std::shared_ptr<NPC> newNpc = std::make_shared<NPC>(Renderer::getShader(ShaderType::basic));
		newNpc->setPosition(glm::vec3(x, 0, z));
		newNpc->setNumber(numObject);
		newNpc->setCurrentTask(CurrentTask::Follow_Character);

		Logger::log("created NPC:" + newNpc->printObject());
		Game::objects.push_back(newNpc);
		Game::npcs.push_back(newNpc);
		Game::characters.push_back(newNpc);
		numObject++;
	}
	Logger::log("Loading all NPCs - finished");

	// set the ids
	int id = 100;
	for (std::shared_ptr<Object> object : Game::objects)
	{
		if (object->getType() & ObjectType::Object_Player)
			continue;
		object->setNetworkID(id);
		id++;
	}

	// ambient sounds
	if (doc.contains("sounds"))
		for (const auto& sound : doc["sounds"])
		loadSound(sound);

	// lights
	if (doc.contains("lights"))
		for (const auto& light : doc["lights"])
		loadLight(light);

	// texts
	if (doc.contains("texts"))
		for (const auto& text : doc["texts"])
		loadText(text);

	Logger::log("Loading Map - finished");
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

void ResourceManager::loadMapProperties(const nlohmann::json& node)
{
	if (node.contains("sun_direction"))
		Map::sun_direction = glm::normalize(jsonVec3(node["sun_direction"]));

	if (node.contains("sun_color"))
		Map::sun_color = jsonVec3(node["sun_color"]);

	if (node.contains("skybox"))
		Map::skyboxName = node["skybox"].get<std::string>();
}

void ResourceManager::loadPlayer(const nlohmann::json& mapNode)
{
	std::string playerModelFileName = "";

	if (mapNode.contains("player") && mapNode["player"].contains("modelfile"))
		playerModelFileName = mapNode["player"]["modelfile"].get<std::string>();

	std::shared_ptr<Player> player = std::make_shared<Player>(Renderer::getShader(ShaderType::basic), glm::radians((float)ConfigManager::fov), Game::getWindowWidth(), Game::getWindowHeight(), playerModelFileName);
	player->setCollisionBoxType(CollisionBoxType::cube);
	player->setName("Player");
	player->setNumber(Game::objects.size());

	if (mapNode.contains("player") && mapNode["player"].contains("position"))
		player->setPosition(jsonVec3(mapNode["player"]["position"]));

	Game::objects.push_back(player);
	Game::players.push_back(player);
	Game::characters.push_back(player);
}

void ResourceManager::loadObject(const nlohmann::json& node, int objectCount)
{
	std::shared_ptr<Object> newObject;

	std::string modelFile = node["modelfile"].get<std::string>();

	if (node.contains("interactable"))
	{
		const nlohmann::json& interact = node["interactable"];

		std::shared_ptr<Button> newButton = std::make_shared<Button>(Renderer::getShader(ShaderType::basic), modelFile);
		newObject = newButton;

		if (interact.contains("scriptfunction_click"))
			newButton->setInteractLuaFunction_click(interact["scriptfunction_click"].get<std::string>());
		if (interact.contains("scriptfunction_hold"))
			newButton->setInteractLuaFunction_hold(interact["scriptfunction_hold"].get<std::string>());
		if (interact.contains("travel"))
			newButton->travel = jsonVec3(interact["travel"]);
		if (interact.contains("travelduration"))
			newButton->travel_duration = interact["travelduration"].get<float>();
	}
	else
	{
		newObject = std::make_shared<Object>(Renderer::getShader(ShaderType::basic), modelFile);
	}

	newObject->setPosition(jsonVec3(node["position"]));
	newObject->setRotation(jsonVec3(node["rotation"]));
	newObject->setScale(jsonVec3(node["scale"]));

	newObject->setType(newObject->getType() | Object::convertStringToType(node["type"].get<std::string>()));
	newObject->setCollisionBoxType(Object::convertStringToCollisionBoxType(node["collisionboxtype"].get<std::string>()));
	newObject->setGravity(node.value("gravity", false));

	newObject->setName(node["name"].get<std::string>());

	if (node.contains("textureflow"))
		newObject->setTextureFlow(jsonVec2(node["textureflow"]));

	if (node.contains("light"))
	{
		const nlohmann::json& light = node["light"];
		PointLight newPointlight;

		if (light.contains("offset"))
			newPointlight.position = newObject->getPosition() + jsonVec3(light["offset"]);

		if (light.contains("color"))
			newPointlight.color = jsonVec3(light["color"]);

		if (light.contains("linear"))
			newPointlight.linear = light["linear"].get<float>();

		if (light.contains("quadratic"))
			newPointlight.quadratic = light["quadratic"].get<float>();

		Map::pointLights.push_back(newPointlight);
	}

	newObject->setNumber(Game::objects.size());

	percentageLoading += 10 / objectCount;
	Logger::log("loaded Object:" + newObject->printObject() + " - " + Helper::to_string_with_precision(percentageLoading, 0) + "%");
	Renderer::loadingProgressBar->setValue(percentageLoading);
	Renderer::drawLoadingScreen();
	Game::objects.push_back(newObject);

	if (newObject->getType() & ObjectType::Object_Environment)
	{
		Game::map.push_back(newObject);
	}
}

void ResourceManager::loadNpc(const nlohmann::json& node, int npcCount)
{
	std::shared_ptr<NPC> newNPC = std::make_shared<NPC>(Renderer::getShader(ShaderType::basic));

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

	newNPC->setNumber(Game::objects.size());

	percentageLoading += 10 / npcCount;
	Logger::log("loaded NPC:" + newNPC->printObject() + " - " + Helper::to_string_with_precision(percentageLoading, 0) + "%");
	Renderer::loadingProgressBar->setValue(percentageLoading);
	Renderer::drawLoadingScreen();

	Game::objects.push_back(newNPC);
	Game::npcs.push_back(newNPC);
	Game::characters.push_back(newNPC);
}

void ResourceManager::loadSound(const nlohmann::json& node)
{
	if (node.contains("soundfile"))
	{
		std::string soundfilename = "audio/";
		soundfilename += node["soundfile"].get<std::string>();

		if (node.contains("position"))
		{
			glm::vec3 position = jsonVec3(node["position"]);
			AudioManager::play3D(soundfilename, position, AudioType::ambient, true);
		}
		else
		{
			AudioManager::play2D(soundfilename, AudioType::ambient, true);
		}
	}
}

void ResourceManager::loadLight(const nlohmann::json& node)
{
	if (node.value("type", "") == "pointlight")
	{
		PointLight newPointlight;

		if (node.contains("position"))
			newPointlight.position = jsonVec3(node["position"]);

		if (node.contains("color"))
			newPointlight.color = jsonVec3(node["color"]);

		if (node.contains("linear"))
			newPointlight.linear = node["linear"].get<float>();

		if (node.contains("quadratic"))
			newPointlight.quadratic = node["quadratic"].get<float>();

		Map::pointLights.push_back(newPointlight);
	}
}

void ResourceManager::loadText(const nlohmann::json& node)
{
	Font3D *newText = new Font3D();

	if (node.contains("text"))
		newText->text = node["text"].get<std::string>();

	if (node.contains("position"))
		newText->position = jsonVec3(node["position"]);

	if (node.contains("rotation"))
		newText->rotation = jsonVec3(node["rotation"]);

	if (node.contains("color"))
		newText->color = jsonVec4(node["color"]);

	if (node.contains("scale"))
		newText->scale = node["scale"].get<float>();

	newText->createTexture();
	Game::texts.push_back(newText);
}

int ResourceManager::loadCubemap(std::vector<std::string> faces)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	stbi_set_flip_vertically_on_load(false);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
						 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}

int ResourceManager::loadImage(std::string fileName)
{
	unsigned int textureID;
	GLCALL(glGenTextures(1, &textureID));
	GLCALL(glBindTexture(GL_TEXTURE_2D, textureID));

	stbi_set_flip_vertically_on_load(true);

	int width, height, nrChannels;

	unsigned char *imageData = stbi_load(fileName.c_str(), &width, &height, &nrChannels, 0);
	if (imageData)
	{
		GLCALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData));

		stbi_image_free(imageData);
	}
	else
	{
		std::cout << "Image " << fileName << " failed to load" << std::endl;
		stbi_image_free(imageData);
	}

	GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

	return textureID;
}
