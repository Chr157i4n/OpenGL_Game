#pragma once
#include "Model.h"
#include "Shader.h"
#include "Player.h"
#include "NPC.h"
#include <fstream>
#include <vector>
#include <cmath>
#include <algorithm>
#include "ConfigManager.h"
#include "Object.h"
#include "tinyxml2.h"

static class ResourceManager
{
public:

	static Shader* loadShader(std::string vertexShaderFilename, std::string fragmentShaderFilename);

	static std::vector<Model*> loadAllModels(tinyxml2::XMLDocument* doc);

	static Model* loadModel(std::string modelFileName);

	static Model* getModelByName(std::string modelFileName);

	static void loadMap(std::string mapFileName, std::vector<std::shared_ptr<Object>>* objects, std::vector<std::shared_ptr<Character>>* characters, std::vector< std::shared_ptr<Player>>* players, std::vector< std::shared_ptr<NPC>>* npcs);

	static void reloadMap(std::string mapFileName, std::vector<std::shared_ptr<Object>>* objects, std::vector<std::shared_ptr<Character>>* characters, std::vector< std::shared_ptr<Player>>* players, std::vector< std::shared_ptr<NPC>>* npcs);


	static int loadCubemap(std::vector<std::string> faces);

	static int loadImage(std::string fileName);



	

private:

	static size_t split(const std::string& txt, std::vector<std::string>& strs, char ch);
	
	static std::string modelFolder;
	static float percentageLoading;

	
};

