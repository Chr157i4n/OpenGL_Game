#pragma once
#include "defines.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <algorithm>
#include "Logger.h"


enum ShadowOption {
	off,
	hard,
	soft,
};

enum FullscreenOption {
	windowed,
	windowed_borderless,
	fullscreen,
};

static class ConfigManager
{

public:

	static void init(std::string nConfigFileName);

	static std::string readConfig(std::string key);

	static void readAllConfigs();

	static void writeConfig(std::string key, std::string value);

	static void writeAllConfigs();

	static ShadowOption shadowOption;
	static FullscreenOption fullscreenOption;
	static int shadowMapResolution;
	static int envMapResolution;
	static float musicVolume;
	static int renderResolutionX, renderResolutionY;

private:

	static size_t split(const std::string& txt, std::vector<std::string>& strs, char ch);

	static std::string trim(std::string& str);

	static std::string configFileName;


};

