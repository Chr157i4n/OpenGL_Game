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

static class ConfigManager
{

public:

	static void init(std::string nConfigFileName);

	static std::string readConfig(std::string key);

	static ShadowOption shadowOption;

private:

	static size_t split(const std::string& txt, std::vector<std::string>& strs, char ch);

	static std::string trim(std::string& str);

	static std::string configFileName;

};

