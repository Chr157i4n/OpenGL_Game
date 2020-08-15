#pragma once
#include "defines.h"

#include "tinyxml2.h"
#include <fstream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <cmath>
#include <sstream>
#include <iostream>
#include <filesystem>

#include "Model.h"


static class ResourceManager
{
public:


	static std::vector<Model*> loadAllModels(std::string modelFileName);

	static Model* loadModel(std::string modelFileName);

	static Model* getModelByName(std::string modelFileName);

	static std::vector<std::string> readAllMaps();

	static void loadMap(std::string mapFileName);

	

private:
	
	static std::string modelFolder;
	static float percentageLoading;

	static std::vector<Model*> models;

	
};

