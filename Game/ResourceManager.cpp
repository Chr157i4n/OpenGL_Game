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

Shader* ResourceManager::loadShader(std::string vertexShaderFilename, std::string fragmentShaderFilename)
{
	Shader* newShader = new Shader(vertexShaderFilename, fragmentShaderFilename);
	newShader->bind();
	return newShader;
}


std::vector<Model*> ResourceManager::loadAllModels(std::string modelFileName)
{
	Renderer::getModels().clear();
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
		Renderer::loadingProgressBar->setValue(percentageLoading);
		Renderer::drawLoadingScreen();

		models.push_back(newModel);
	}

	Renderer::setModels(models);

	Logger::log("Loading all Models - finished");
	return models;
}

Model* ResourceManager::loadModel(std::string modelFileName)
{
	Model* newModel = new Model();

	const char* fileNameChar = modelFileName.c_str();
	if (!newModel->init(fileNameChar, Renderer::getShader(ShaderType::basic)))
	{
		return nullptr;
	}

	return newModel;
}

Model* ResourceManager::getModelByName(std::string modelFileName)
{
	for (Model* model : Renderer::getModels())
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
	Game::players.clear();
	Game::npcs.clear();
	Game::bullets.clear();

	Game::texts.clear();
	Map::pointLights.clear();

	const char* mapFileNameC = mapFileName.c_str();
	int32 numObject = 0;
	tinyxml2::XMLDocument doc;
	std::string xmlNodeText;

	Logger::log("Loading Map");
	doc.LoadFile(mapFileNameC);

	tinyxml2::XMLElement* xmlNode;
	tinyxml2::XMLElement* mapNode;
	
	mapNode = doc.FirstChildElement("map");

	if (!mapNode)
	{
		Logger::log("Map file has now 'map' Node.");
		return;
	}

	//meta infos
	xmlNode = mapNode->FirstChildElement("name");
	if (xmlNode)
	{
		Map::mapName =xmlNode->GetText();
	}

	//map properties
	tinyxml2::XMLElement* xmlNodeMapProperties = mapNode->FirstChildElement("map_properties");
	if (xmlNodeMapProperties != NULL)
	{
		loadMapProperties(xmlNodeMapProperties);
	}


	//Player(s)
	loadPlayer(mapNode);



	Logger::log("Loading all Objects");
	float objectCount = 0;
	for (tinyxml2::XMLElement* xmlNodeObject = mapNode->FirstChildElement("objects")->FirstChildElement("object"); xmlNodeObject != NULL; xmlNodeObject = xmlNodeObject->NextSiblingElement())
	{
		objectCount++;
	}


	for (tinyxml2::XMLElement* xmlNodeObject = mapNode->FirstChildElement("objects")->FirstChildElement("object"); xmlNodeObject != NULL; xmlNodeObject = xmlNodeObject->NextSiblingElement())
	{
		loadObject(xmlNodeObject, objectCount);
	}
	Logger::log("Loading all Objects - finished");



	Logger::log("Loading all NPCs");
	float npcCount = 0;
	for (tinyxml2::XMLElement* xmlNodeBot = mapNode->FirstChildElement("bots")->FirstChildElement("bot"); xmlNodeBot != NULL; xmlNodeBot = xmlNodeBot->NextSiblingElement())
	{
		npcCount++;
	}

	for (tinyxml2::XMLElement* xmlNodeBot = mapNode->FirstChildElement("bots")->FirstChildElement("bot"); xmlNodeBot != NULL; xmlNodeBot = xmlNodeBot->NextSiblingElement())
	{
		loadNpc(xmlNodeBot, npcCount);
	}



	//random bots
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


	//set the ids
	int id = 100;
	for (std::shared_ptr<Object> object : Game::objects)
	{
		if (object->getType() & ObjectType::Object_Player) continue;
		object->setNetworkID(id);
		id++;
	}





	//ambient sounds
	tinyxml2::XMLElement* xmlNodesounds = mapNode->FirstChildElement("sounds");
	if (xmlNodesounds != NULL)
	{
		for (tinyxml2::XMLElement* xmlNodesound = xmlNodesounds->FirstChildElement("sound"); xmlNodesound != NULL; xmlNodesound = xmlNodesound->NextSiblingElement())
		{
			loadSound(xmlNodesound);
		}
	}

	//lights
	tinyxml2::XMLElement* xmlNodelights = mapNode->FirstChildElement("lights");
	if (xmlNodelights != NULL)
	{
		for (tinyxml2::XMLElement* xmlNodelight = xmlNodelights->FirstChildElement("light"); xmlNodelight != NULL; xmlNodelight = xmlNodelight->NextSiblingElement())
		{
			loadLight(xmlNodelight);
		}
	}

	//texts
	tinyxml2::XMLElement* xmlNodetexts = mapNode->FirstChildElement("texts");
	if (xmlNodetexts != NULL)
	{
		for (tinyxml2::XMLElement* xmlNodetext = xmlNodetexts->FirstChildElement("text"); xmlNodetext != NULL; xmlNodetext = xmlNodetext->NextSiblingElement())
		{
			loadText(xmlNodetext);
		}
	}



	Logger::log("Loading Map - finished");
}

void ResourceManager::loadMapProperties(tinyxml2::XMLElement* xmlNode)
{
	tinyxml2::XMLElement* xmlNodeSub = xmlNode->FirstChildElement("sun_direction");
	if (xmlNodeSub)
	{
		Map::sun_direction = glm::normalize(glm::vec3(Helper::string_to_glmVec3(xmlNodeSub->GetText())));
	}

	xmlNodeSub = xmlNode->FirstChildElement("sun_color");
	if (xmlNodeSub)
	{
		Map::sun_color = glm::vec3(Helper::string_to_glmVec3(xmlNodeSub->GetText()));
	}

	xmlNodeSub = xmlNode->FirstChildElement("skybox");
	if (xmlNodeSub)
	{
		Map::skyboxName = xmlNodeSub->GetText();
	}
}

void ResourceManager::loadPlayer(tinyxml2::XMLElement* xmlNode)
{
	std::shared_ptr<Player> player = std::make_shared<Player>(Renderer::getShader(ShaderType::basic), glm::radians((float)ConfigManager::fov), Game::getWindowWidth(), Game::getWindowHeight());
	player->setCollisionBoxType(CollisionBoxType::cube);
	player->setName("Player");
	player->setNumber(Game::objects.size());

	tinyxml2::XMLElement* xmlNodePlayer = xmlNode->FirstChildElement("player");
	if (xmlNodePlayer != NULL)
	{
		tinyxml2::XMLElement* xmlNodeSub = xmlNodePlayer->FirstChildElement("position");
		if (xmlNodeSub)
		{
			player->setPosition(Helper::string_to_glmVec3(xmlNodeSub->GetText()));
		}
	}



	Game::objects.push_back(player);
	Game::players.push_back(player);
	Game::characters.push_back(player);
}

void ResourceManager::loadObject(tinyxml2::XMLElement* xmlNode, int objectCount)
{
	std::string xmlNodeText;
	
	std::shared_ptr<Object> newObject;

	xmlNodeText = xmlNode->FirstChildElement("modelfile")->GetText();

	if (xmlNode->FirstChildElement("interactable"))
	{
		tinyxml2::XMLElement* xmlNodeInteract = xmlNode->FirstChildElement("interactable");

		std::shared_ptr<Button> newButton = std::make_shared<Button>(Renderer::getShader(ShaderType::basic), xmlNodeText);
		newObject = newButton;

		if (xmlNodeInteract->FirstChildElement("scriptfunction_click"))
		{
			xmlNodeText = xmlNodeInteract->FirstChildElement("scriptfunction_click")->GetText();
			newButton->setInteractLuaFunction_click(xmlNodeText);
		}
		if (xmlNodeInteract->FirstChildElement("scriptfunction_hold"))
		{
			xmlNodeText = xmlNodeInteract->FirstChildElement("scriptfunction_hold")->GetText();
			newButton->setInteractLuaFunction_hold(xmlNodeText);
		}
		if (xmlNodeInteract->FirstChildElement("travel"))
		{
			xmlNodeText = xmlNodeInteract->FirstChildElement("travel")->GetText();
			newButton->travel = Helper::string_to_glmVec3(xmlNodeText);
		}
		if (xmlNodeInteract->FirstChildElement("travelduration"))
		{
			xmlNodeText = xmlNodeInteract->FirstChildElement("travelduration")->GetText();
			newButton->travel_duration = std::stof(xmlNodeText);
		}
	}
	else
	{
		newObject = std::make_shared<Object>(Renderer::getShader(ShaderType::basic), xmlNodeText);
	}

	xmlNodeText = xmlNode->FirstChildElement("position")->GetText();
	newObject->setPosition(Helper::string_to_glmVec3(xmlNodeText));

	xmlNodeText = xmlNode->FirstChildElement("rotation")->GetText();
	newObject->setRotation(Helper::string_to_glmVec3(xmlNodeText));

	xmlNodeText = xmlNode->FirstChildElement("scale")->GetText();
	newObject->setScale(Helper::string_to_glmVec3(xmlNodeText));

	xmlNodeText = xmlNode->FirstChildElement("type")->GetText();
	newObject->setType(newObject->getType() | Object::convertStringToType(xmlNodeText));

	xmlNodeText = xmlNode->FirstChildElement("collisionboxtype")->GetText();
	newObject->setCollisionBoxType(Object::convertStringToCollisionBoxType(xmlNodeText));

	xmlNodeText = xmlNode->FirstChildElement("gravity")->GetText();
	if (xmlNodeText == "true") { newObject->setGravity(true); }
	else { newObject->setGravity(false); }

	xmlNodeText = xmlNode->FirstChildElement("name")->GetText();
	newObject->setName(xmlNodeText);

	if (xmlNode->FirstChildElement("textureflow"))
	{
		xmlNodeText = xmlNode->FirstChildElement("textureflow")->GetText();
		newObject->setTextureFlow(Helper::string_to_glmVec2(xmlNodeText));
	}

	if (xmlNode->FirstChildElement("light"))
	{
		tinyxml2::XMLElement* xmlNodelight = xmlNode->FirstChildElement("light");
		PointLight newPointlight;

		if (xmlNodelight->FirstChildElement("offset"))
		{
			xmlNodeText = xmlNodelight->FirstChildElement("offset")->GetText();
			newPointlight.position = newObject->getPosition() + Helper::string_to_glmVec3(xmlNodeText);
		}

		if (xmlNodelight->FirstChildElement("color"))
		{
			xmlNodeText = xmlNodelight->FirstChildElement("color")->GetText();
			newPointlight.color = Helper::string_to_glmVec3(xmlNodeText);
		}

		if (xmlNodelight->FirstChildElement("linear"))
		{
			xmlNodeText = xmlNodelight->FirstChildElement("linear")->GetText();
			newPointlight.linear = stof(xmlNodeText);
		}

		if (xmlNodelight->FirstChildElement("quadratic"))
		{
			xmlNodeText = xmlNodelight->FirstChildElement("quadratic")->GetText();
			newPointlight.quadratic = stof(xmlNodeText);
		}

		Map::pointLights.push_back(newPointlight);
	}

	if (xmlNode->FirstChildElement("sound"))
	{
		//todo
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

void ResourceManager::loadNpc(tinyxml2::XMLElement* xmlNode, int npcCount)
{
	std::string xmlNodeText;

	std::shared_ptr<NPC> newNPC = std::make_shared<NPC>(Renderer::getShader(ShaderType::basic));

	xmlNodeText = xmlNode->FirstChildElement("modelfile")->GetText();
	newNPC->setModel(xmlNodeText);

	xmlNodeText = xmlNode->FirstChildElement("position")->GetText();
	newNPC->setPosition(Helper::string_to_glmVec3(xmlNodeText));

	xmlNodeText = xmlNode->FirstChildElement("rotation")->GetText();
	newNPC->setRotation(Helper::string_to_glmVec3(xmlNodeText));

	xmlNodeText = xmlNode->FirstChildElement("scale")->GetText();
	newNPC->setScale(Helper::string_to_glmVec3(xmlNodeText));


	xmlNodeText = xmlNode->FirstChildElement("name")->GetText();
	newNPC->setName(xmlNodeText);

	xmlNodeText = xmlNode->FirstChildElement("team")->GetText();
	newNPC->setTeam(std::stoi(xmlNodeText));

	if (xmlNode->FirstChildElement("navpoints"))
	{
		if (xmlNode->FirstChildElement("navpoints")->FirstChildElement("navpoint"))
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


		for (tinyxml2::XMLElement* xmlNodeNavPoint = xmlNode->FirstChildElement("navpoints")->FirstChildElement("navpoint"); xmlNodeNavPoint != NULL; xmlNodeNavPoint = xmlNodeNavPoint->NextSiblingElement())
		{
			xmlNodeText = xmlNodeNavPoint->GetText();
			newNPC->addNavPoint(Helper::string_to_glmVec3(xmlNodeText));
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

void ResourceManager::loadSound(tinyxml2::XMLElement* xmlNode)
{
	if (xmlNode->FirstChildElement("soundfile"))
	{
		std::string soundfilename = "audio\\";
		soundfilename += xmlNode->FirstChildElement("soundfile")->GetText();

		tinyxml2::XMLElement* xmlNodeSub = xmlNode->FirstChildElement("position");
		if (xmlNodeSub)
		{
			glm::vec3 position = glm::vec3(Helper::string_to_glmVec3(xmlNodeSub->GetText()));
			irrklang::ISound* sound = AudioManager::play3D(soundfilename, position, AudioType::ambient, true);
		}
		else
		{
			irrklang::ISound* sound = AudioManager::play2D(soundfilename, AudioType::ambient, true);
		}
	}
}

void ResourceManager::loadLight(tinyxml2::XMLElement* xmlNode)
{
	if (!xmlNode) return;

	std::string xmlNodeText = "";

	if (xmlNode->FirstChildElement("type")->GetText() == "pointlight")
	{
		PointLight newPointlight;

		if (xmlNode->FirstChildElement("position"))
		{
			xmlNodeText = xmlNode->FirstChildElement("position")->GetText();
			newPointlight.position =Helper::string_to_glmVec3(xmlNodeText);
		}

		if (xmlNode->FirstChildElement("color"))
		{
			xmlNodeText = xmlNode->FirstChildElement("color")->GetText();
			newPointlight.color = Helper::string_to_glmVec3(xmlNodeText);
		}

		if (xmlNode->FirstChildElement("linear"))
		{
			xmlNodeText = xmlNode->FirstChildElement("linear")->GetText();
			newPointlight.linear = stof(xmlNodeText);
		}

		if (xmlNode->FirstChildElement("quadratic"))
		{
			xmlNodeText = xmlNode->FirstChildElement("quadratic")->GetText();
			newPointlight.quadratic = stof(xmlNodeText);
		}

		Map::pointLights.push_back(newPointlight);
	}

}

void ResourceManager::loadText(tinyxml2::XMLElement* xmlNode)
{
	if (!xmlNode) return;

	std::string xmlNodeText = "";
	
	Font3D* newText = new Font3D();

	if (xmlNode->FirstChildElement("text"))
	{
		xmlNodeText = xmlNode->FirstChildElement("text")->GetText();
		newText->text = xmlNodeText;
	}

	if (xmlNode->FirstChildElement("position"))
	{
		xmlNodeText = xmlNode->FirstChildElement("position")->GetText();
		newText->position = Helper::string_to_glmVec3(xmlNodeText);
	}

	if (xmlNode->FirstChildElement("rotation"))
	{
		xmlNodeText = xmlNode->FirstChildElement("rotation")->GetText();
		newText->rotation = Helper::string_to_glmVec3(xmlNodeText);
	}

	if (xmlNode->FirstChildElement("color"))
	{
		xmlNodeText = xmlNode->FirstChildElement("color")->GetText();
		newText->color = Helper::string_to_glmVec4(xmlNodeText);
	}

	if (xmlNode->FirstChildElement("scale"))
	{
		xmlNodeText = xmlNode->FirstChildElement("scale")->GetText();
		newText->scale = std::stof(xmlNodeText);
	}

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
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
			);
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

	unsigned char* imageData = stbi_load(fileName.c_str(), &width, &height, &nrChannels, 0);
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