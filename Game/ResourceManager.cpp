#include "ResourceManager.h"

#include "Logger.h"
#include "Renderer.h"
#include "Player.h"
#include "NPC.h"
#include "ConfigManager.h"
#include "Object.h"
#include "Game.h"

std::string ResourceManager::modelFolder = "models";
float ResourceManager::percentageLoading = 0;


template <typename T>
std::string to_string_with_precision(const T a_value, const int n = 6)
{
	std::ostringstream out;
	out.precision(n);
	out << std::fixed << a_value;
	return out.str();
}

Shader* ResourceManager::loadShader(std::string vertexShaderFilename, std::string fragmentShaderFilename)
{
	Shader* newShader = new Shader(vertexShaderFilename, fragmentShaderFilename);
	newShader->bind();
	return newShader;
}


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
		Logger::log("loaded Model " + std::to_string(newModel->getModelID()) + ": " + newModel->getModelName() + " - " + to_string_with_precision(percentageLoading, 0) + "%");
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
	std::vector<std::shared_ptr<Object>>* map = &Game::map;
	std::vector<std::shared_ptr<Object>>* objects = &Game::objects;
	std::vector<std::shared_ptr<Character>>* characters = &Game::characters;
	std::vector< std::shared_ptr<Player>>* players = &Game::players;
	std::vector< std::shared_ptr<NPC>>* npcs = &Game::npcs;

	const char* mapFileNameC = mapFileName.c_str();
	int32 numObject = 0;
	tinyxml2::XMLDocument doc;
	std::string xmlNodeText;
	std::vector<std::string> params;

	Logger::log("Loading Map");
	doc.LoadFile(mapFileNameC);

	std::string title = doc.FirstChildElement("map")->FirstChildElement("name")->GetText();

	//Player(s)
	float fov = std::stof(ConfigManager::readConfig("fov"));
	std::shared_ptr<Player> player = std::make_shared<Player>(Renderer::getShader(ShaderType::basic), glm::radians(fov), 1920, 1080);
	player->setCollisionBoxType(CollisionBoxType::cube);
	player->setName("Player");
	player->setNumber(numObject);
	objects->push_back(player);
	players->push_back(player);
	characters->push_back(player);
	numObject++;



	Logger::log("Loading all Objects");
	float objectCount = 0;
	for (tinyxml2::XMLElement* xmlNodeObject = doc.FirstChildElement("map")->FirstChildElement("objects")->FirstChildElement("object"); xmlNodeObject != NULL; xmlNodeObject = xmlNodeObject->NextSiblingElement())
	{
		objectCount++;
	}

	for (tinyxml2::XMLElement* xmlNodeObject = doc.FirstChildElement("map")->FirstChildElement("objects")->FirstChildElement("object"); xmlNodeObject != NULL; xmlNodeObject = xmlNodeObject->NextSiblingElement())
	{
		xmlNodeText = xmlNodeObject->FirstChildElement("modelfile")->GetText();

		std::shared_ptr<Object> newObject = std::make_shared<Object>(Renderer::getShader(ShaderType::basic), xmlNodeText);

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

		xmlNodeText = xmlNodeObject->FirstChildElement("collisionboxtype")->GetText();
		newObject->setCollisionBoxType(Object::convertStringToCollisionBoxType(xmlNodeText));

		xmlNodeText = xmlNodeObject->FirstChildElement("gravity")->GetText();
		if (xmlNodeText == "true") { newObject->setGravity(true); }
		else { newObject->setGravity(false); }

		xmlNodeText = xmlNodeObject->FirstChildElement("name")->GetText();
		newObject->setName(xmlNodeText);

		if (xmlNodeObject->FirstChildElement("textureflow"))
		{
			xmlNodeText = xmlNodeObject->FirstChildElement("textureflow")->GetText();
			split(xmlNodeText, params, ';');
			newObject->setTextureFlow( glm::vec2(stof(params[0]), stof(params[1])) );
		}

		newObject->setNumber(numObject);

		percentageLoading += 10 / objectCount;
		Logger::log("loaded Object:" + newObject->printObject() + " - " + to_string_with_precision(percentageLoading, 0) + "%");
		Renderer::loadingProgressBar->setValue(percentageLoading);
		Renderer::drawLoadingScreen();
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


		std::shared_ptr<NPC> newNPC = std::make_shared<NPC>(Renderer::getShader(ShaderType::basic));

		xmlNodeText = xmlNodeBot->FirstChildElement("modelfile")->GetText();
		newNPC->setModel(xmlNodeText);

		xmlNodeText = xmlNodeBot->FirstChildElement("position")->GetText();
		split(xmlNodeText, params, ';');
		newNPC->setPosition(glm::vec3(stof(params[0]), stof(params[1]), stof(params[2])));

		xmlNodeText = xmlNodeBot->FirstChildElement("rotation")->GetText();
		split(xmlNodeText, params, ';');
		newNPC->setRotation(glm::vec3(stof(params[0]), stof(params[1]), stof(params[2])));

		xmlNodeText = xmlNodeBot->FirstChildElement("scale")->GetText();
		split(xmlNodeText, params, ';');
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
				split(xmlNodeText, params, ';');
				newNPC->addNavPoint(glm::vec3(stof(params[0]), stof(params[1]), stof(params[2])));
			}
		}


		newNPC->setNumber(numObject);

		percentageLoading += 10 / npcCount;
		Logger::log("loaded NPC:" + newNPC->printObject() + " - " + to_string_with_precision(percentageLoading, 0) + "%");
		Renderer::loadingProgressBar->setValue(percentageLoading);
		Renderer::drawLoadingScreen();

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

		std::shared_ptr<NPC> newNpc = std::make_shared<NPC>(Renderer::getShader(ShaderType::basic));
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

	Logger::log("Loading Map - finished");
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
	strs.push_back(txt.substr(initialPos, (std::min)(pos, txt.size()) - initialPos + 1));

	return strs.size();
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