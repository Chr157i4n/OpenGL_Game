#pragma once
#include "defines.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <string>

#include "Logger.h"


static class ConfigManager
{

public:

	static void init(std::string nConfigFileName);

	static std::string readConfig(std::string key);

	static void readAllConfigs();

	static void writeConfig(std::string key, std::string value);

	static void writeAllConfigs();

	//SETTINGS
	//[General]
	static std::string level;
	static int bots;
	static float bot_speed_mult;
	static int max_bullets;
	static int fps_limit_server;
	//[Network]
	static int server_port;	
	

private:

	static size_t split(const std::string& txt, std::vector<std::string>& strs, char ch);

	static std::string trim(std::string& str);

	static std::string configFileName;


};

