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

class ResourceManager
{
public:
	ResourceManager(ConfigManager* configManager);

	void loadShader(std::string vertexShaderFilename, std::string fragmentShaderFilename);

	Shader* getShader();

	void bindShader();

	void unbindShader();

	void loadMap(std::string mapFileName, std::vector<Object*>* objects, std::vector<Player*>* players, std::vector<NPC*>* npcs);

private:

	size_t split(const std::string& txt, std::vector<std::string>& strs, char ch);



	Shader* shader;
	ConfigManager* configManager;
};

