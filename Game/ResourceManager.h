#pragma once
#include "defines.h"

#include <nlohmann/json.hpp>
#include <fstream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <stb_image.h>
#include <cmath>
#include <sstream>
#include <iostream>
#include <filesystem>

#include "Shader.h"
#include "Model.h"


class ResourceManager
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
	static void loadDefaultModels(const nlohmann::json& node);

	static void loadMapProperties(const nlohmann::json& node);

	static void loadPlayer(const nlohmann::json& mapNode);

	static void loadObject(const nlohmann::json& node, int objectCount);

	static void loadNpc(const nlohmann::json& node, int npcCount);

	static void loadSound(const nlohmann::json& node);

	static void loadLight(const nlohmann::json& node);

	static void loadText(const nlohmann::json& node);


	
	//
	//
	static int loadCubemap(std::vector<std::string> faces);

	static int loadImage(std::string fileName);



private:

	
	static std::string modelFolder;
	static float percentageLoading;

	
};

