#pragma once
#include "defines.h"

#include "tinyxml2.h"
#include <fstream>
#include <vector>
#include <cmath>
#include <algorithm>
#include "libs/stb_image.h"
#include <cmath>
#include <sstream>
#include <iostream>
#include <filesystem>

#include "Shader.h"
#include "Model.h"


static class ResourceManager
{
public:

	static Shader* loadShader(std::string vertexShaderFilename, std::string fragmentShaderFilename);

	static std::vector<Model*> loadAllModels(std::string modelFileName);

	static Model* loadModel(std::string modelFileName);

	static Model* getModelByName(std::string modelFileName);




	static std::vector<std::string> readAllMaps();

	static void loadMap(std::string mapFileName);

	//parts of map loading
	//
	static void loadMapProperties(tinyxml2::XMLElement* xmlNode);

	static void loadPlayer(tinyxml2::XMLElement* xmlNode);

	static void loadObject(tinyxml2::XMLElement* xmlNode, int objectCount);

	static void loadNpc(tinyxml2::XMLElement* xmlNode, int npcCount);

	static void loadSound(tinyxml2::XMLElement* xmlNode);

	static void loadLight(tinyxml2::XMLElement* xmlNode);

	static void loadText(tinyxml2::XMLElement* xmlNode);


	
	//
	//
	static int loadCubemap(std::vector<std::string> faces);

	static int loadImage(std::string fileName);



private:

	
	static std::string modelFolder;
	static float percentageLoading;

	
};

