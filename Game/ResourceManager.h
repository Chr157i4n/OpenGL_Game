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


	static int loadCubemap(std::vector<std::string> faces);

	static int loadImage(std::string fileName);



	

private:

	static size_t split(const std::string& txt, std::vector<std::string>& strs, char ch);
	
	static std::string modelFolder;
	static float percentageLoading;

	
};

