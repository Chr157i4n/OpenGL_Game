#include "ConfigManager.h"

std::string ConfigManager::configFileName;
ShadowOption ConfigManager::shadowOption;
int ConfigManager::shadowMapResolution = 1024;
int ConfigManager::envMapResolution = 1024;
float ConfigManager::musicVolume=0.4;


void ConfigManager::init(std::string nConfigFileName)
{
	configFileName = nConfigFileName;

	ConfigManager::readAllConfigs();
}

std::string ConfigManager::readConfig(std::string key)
{
	std::ifstream configFile;
	configFile.open(configFileName);

	if (!configFile) {
		Logger::log("Cant open Configfile: " + configFileName);
		return "";
	}

	char line[255];

	while (!configFile.eof())
	{
		std::vector<std::string> param;
		configFile.getline(line, 255);

		if (line[0] == '\0' || line[0] == '#')
		{
			continue;
		}

		split(line, param, '=');

		for (int i = 0; i < param.size(); i++)
		{
			param[i] = trim(param[i]);
		}

		if (param[0] == key)
		{
			return param[1];
		}

	}
}

void ConfigManager::readAllConfigs()
{
	ConfigManager::shadowMapResolution = std::stoi(ConfigManager::readConfig("shadow_map_resolution"));
	ConfigManager::envMapResolution = std::stoi(ConfigManager::readConfig("env_map_resolution"));
	ConfigManager::musicVolume = std::stof(ConfigManager::readConfig("music_volume"));
}

void ConfigManager::writeConfig(std::string key, std::string value)
{
	std::ifstream filein(configFileName); //File to read from
	std::ofstream fileout("_"+ configFileName); //Temporary file
	if (!filein || !fileout)
	{
		Logger::log("Error opening configfiles for saving!");
		return;
	}

	char linein[255];
	std::string lineout;

	while (!filein.eof())
	{
		std::vector<std::string> param;
		filein.getline(linein, 255);

		if (linein[0] == '\0' || linein[0] == '#')	continue;

		split(linein, param, '=');

		for (int i = 0; i < param.size(); i++)
		{
			param[i] = trim(param[i]);
		}

		if (param[0] == key)
		{
			lineout = key + " = " + value;
		}
		else 
		{
			lineout = linein;
		}

		fileout << lineout << '\n';

	}

	filein.close();
	fileout.close();

	std::remove(configFileName.c_str());
	std::rename(("_" + configFileName).c_str(), configFileName.c_str());
}

void ConfigManager::writeAllConfigs()
{
	ConfigManager::writeConfig("shadow_map_resolution",		std::to_string(ConfigManager::shadowMapResolution));
	ConfigManager::writeConfig("env_map_resolution",		std::to_string(ConfigManager::envMapResolution));
	ConfigManager::writeConfig("music_volume",				std::to_string(ConfigManager::musicVolume));
}

size_t ConfigManager::split(const std::string& txt, std::vector<std::string>& strs, char ch)
	{
		size_t pos = txt.find(ch);
		size_t initialPos = 0;
		strs.clear();

		// Decompose statement
		while (pos != std::string::npos) {
			strs.push_back(txt.substr(initialPos, pos - initialPos));
			initialPos = pos + 1;

			pos = txt.find(ch, initialPos);
		}

		// Add the last one
		strs.push_back(txt.substr(initialPos, std::min(pos, txt.size()) - initialPos + 1));

		return strs.size();
	}

std::string ConfigManager::trim(std::string& str)
	{
		size_t first = str.find_first_not_of(' ');
		size_t last = str.find_last_not_of(' ');
		return str.substr(first, (last - first + 1));
	}