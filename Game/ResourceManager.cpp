#include "ResourceManager.h"

#include "Logger.h"
#include "Renderer.h"

#include "libs/stb_image.h"


std::string ResourceManager::modelFolder = "models";
Shader* ResourceManager::shaderBasic;

void ResourceManager::init()
{
	shaderBasic = loadShader("shaders/basic.vert", "shaders/basic.frag");
}


Shader* ResourceManager::loadShader(std::string vertexShaderFilename, std::string fragmentShaderFilename)
{
	Shader* newShader = new Shader(vertexShaderFilename, fragmentShaderFilename);
	newShader->bind();
	return newShader;
}

Shader* ResourceManager::getShaderBasic()
{
	return shaderBasic;
}

void ResourceManager::bindShaderBasic()
{
	shaderBasic->bind();
}

void ResourceManager::unbindShaderBasic()
{
	shaderBasic->unbind();
}


std::vector<Model*> ResourceManager::loadModels(tinyxml2::XMLDocument* doc)
{
	std::vector<Model*> models;
	
	std::string xmlNodeText;
	int id = 0;

	for (tinyxml2::XMLElement* xmlNodeObject = doc->FirstChildElement("map")->FirstChildElement("models")->FirstChildElement("model"); xmlNodeObject != NULL; xmlNodeObject = xmlNodeObject->NextSiblingElement())
	{
		xmlNodeText = xmlNodeObject->GetText();

		Model* newModel = loadModel(modelFolder + "/" + xmlNodeText);
		newModel->setModelName(xmlNodeText);
		newModel->setModelID(id++);


		models.push_back(newModel);
	}

	return models;
}

Model* ResourceManager::loadModel(std::string modelFileName)
{
	Model* newModel = new Model();

	const char* fileNameChar = modelFileName.c_str();
	newModel->init(fileNameChar, shaderBasic);

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

	std::vector<Model*> models = loadModels(&doc);
	Renderer::setModels(models);

	//Player(s)
	float fov = std::stof(ConfigManager::readConfig("fov"));
	Player* player = new Player(shaderBasic, fov, 800.0f, 600.0f);
	player->setCollisionBoxType(CollisionBoxType::cube);
	player->setName("Player");
	player->setNumber(numObject);
	objects->push_back(player);
	players->push_back(player);
	characters->push_back(player);
	numObject++;
	

	

	for (tinyxml2::XMLElement* xmlNodeObject = doc.FirstChildElement("map")->FirstChildElement("objects")->FirstChildElement("object"); xmlNodeObject != NULL; xmlNodeObject = xmlNodeObject->NextSiblingElement())
	{
		xmlNodeText = xmlNodeObject->FirstChildElement("modelfile")->GetText();

		Object* newObject = new Object(shaderBasic, xmlNodeText);

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

		newObject->setNumber(numObject);
		objects->push_back(newObject);

		numObject++;
	}



	for (tinyxml2::XMLElement* xmlNodeBot = doc.FirstChildElement("map")->FirstChildElement("bots")->FirstChildElement("bot"); xmlNodeBot != NULL; xmlNodeBot = xmlNodeBot->NextSiblingElement())
	{
		xmlNodeText = xmlNodeBot->FirstChildElement("modelfile")->GetText();

		NPC* newNPC = new NPC(shaderBasic);

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

		NPC* newNpc = new NPC(shaderBasic);
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