#include "Map.h"

#include "UI.h"
#include "ResourceManager.h"
#include "Game.h"

std::string Map::mapName = "";
std::string Map::mapFileName = "";
std::string Map::mapFileFolder = "levels\\";
std::string Map::mapFileExtension = ".xml";

glm::vec3 Map::sun_direction= glm::vec3(-0.8, +0.4, +0.4);
glm::vec3 Map::sun_color = glm::vec3(0.8, 0.8, 0.8);
std::string Map::skyboxName = "skybox1";

std::vector<PointLight> Map::pointLights;

glm::vec3 Map::gravity = glm::vec3(0, -80, 0);

glm::vec2 Map::mapSize = glm::vec2(110,110);

unsigned int Map::skyboxTexture;

std::vector<irrklang::ISound*> Map::ambientSounds;

void Map::load(std::string mapFileName)
{
	if (mapFileName.find(mapFileFolder) == std::string::npos) {
		mapFileName = mapFileFolder+mapFileName;
	}
	if (mapFileName.find(mapFileExtension) == std::string::npos) {
		mapFileName = mapFileName+mapFileExtension;
	}
	
	Map::mapFileName = mapFileName;
	
	ResourceManager::loadAllModels(mapFileName);
	ResourceManager::loadMap(mapFileName);
	Renderer::loadingProgressBar->setLifespan(-1);

	Map::mapSize.x = Game::map[0]->getDimensions().x;
	Map::mapSize.y = Game::map[0]->getDimensions().z;

	Map::loadSkybox();

	Renderer::initLight();
}

void Map::restart()
{	
	std::vector<unsigned int> envCubeMaps, envCubeMapFrameBuffers;

	for (std::shared_ptr<Object> object : Game::objects)
	{
		envCubeMaps.push_back(object->getEnvCubeMap());
		envCubeMapFrameBuffers.push_back(object->getEnvCubeMapFrameBuffer());
	}


	Game::objects.clear();
	Game::characters.clear();
	Game::players.clear();
	Game::npcs.clear();

	ResourceManager::loadMap(mapFileName);

	for (std::shared_ptr<Object> object : Game::objects)
	{
		object->setHealth(100);
	}

	UI::clearUI_Elements();

	Game::players[0]->createHealthbar();

	//Renderer::resetFrameCount();
	//todo: if object dies, this doesnt work anymore. perhaps died objects should only be "disabled" instead of deleted
	for (int i = 0; i < Game::objects.size(); i++)
	{
		if(i < envCubeMaps.size())
			Game::objects[i]->setEnvCubeMap(envCubeMaps[i]);
		if (i < envCubeMapFrameBuffers.size())
			Game::objects[i]->setEnvCubeMapFrameBuffer(envCubeMapFrameBuffers[i]);
	}

	Game::setGameState(GameState::GAME_ACTIVE);
	SDL_SetRelativeMouseMode(SDL_TRUE);
}

std::vector<std::string> Map::getSkyboxFaces()
{
	std::vector<std::string> faces;

	faces.push_back("textures/skybox/" + skyboxName + "_right.jpg");
	faces.push_back("textures/skybox/" + skyboxName + "_left.jpg");
	faces.push_back("textures/skybox/" + skyboxName + "_top.jpg");
	faces.push_back("textures/skybox/" + skyboxName + "_bottom.jpg");
	faces.push_back("textures/skybox/" + skyboxName + "_front.jpg");
	faces.push_back("textures/skybox/" + skyboxName + "_back.jpg");

	return faces;
}

void Map::loadSkybox()
{
	Logger::log("Load Skybox: " + Map::skyboxName);
	std::vector<std::string> faces = Map::getSkyboxFaces();
	skyboxTexture = ResourceManager::loadCubemap(faces);
}
