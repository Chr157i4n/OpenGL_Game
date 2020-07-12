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

	static void init();

	static void loadShader(std::string vertexShaderFilename, std::string fragmentShaderFilename);

	static Shader* getObjectShader();

	static void bindShader();

	static void unbindShader();

	static void loadMap(std::string mapFileName, std::vector<Object*>* objects, std::vector<Character*>* characters, std::vector<Player*>* players, std::vector<NPC*>* npcs);

private:

	static size_t split(const std::string& txt, std::vector<std::string>& strs, char ch);



	static Shader* shader;
	static std::string modelFolder;
};

