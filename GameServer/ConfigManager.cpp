#include "ConfigManager.h"



std::string			ConfigManager::configFileName = "config.ini";

//SETTINGS
//[General]
std::string			ConfigManager::level = "level_test.xml";
int					ConfigManager::bots = 0;
float				ConfigManager::bot_speed_mult = 0.5;
int					ConfigManager::max_bullets = 20;
int					ConfigManager::fps_limit_server = 60;
//[Network]
int					ConfigManager::server_port= 12975;


void ConfigManager::init(std::string nConfigFileName)
{
	configFileName = nConfigFileName;

	ConfigManager::readAllConfigs();
}

std::string ConfigManager::readConfig(std::string key)
{
	Logger::log("reading Configuration key: " + key);
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


		if (line[0] == '\0' ||	//	line is empty
			line[0] == '#' ||	//	line is comment
			line[0] == ';' ||	//	line is comment
			line[0] == '[')		//	line is section
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

	Logger::log("While reading config did not find this key: " + key);
	return "";
}

void ConfigManager::readAllConfigs()
{
	Logger::log("Reading Configuration");
	std::string config = "";

	//[General]

	config = ConfigManager::readConfig("level");
	if (config != "") ConfigManager::level = config;

	config = ConfigManager::readConfig("bots");
	if (config != "") ConfigManager::bots = std::stoi(config);

	config = ConfigManager::readConfig("bot_speed_mult");
	if (config != "") ConfigManager::bot_speed_mult = std::stof(config);

	config = ConfigManager::readConfig("max_bullets");
	if (config != "") ConfigManager::max_bullets = std::stoi(config);

	config = ConfigManager::readConfig("fps_limit_server");
	if (config != "") ConfigManager::fps_limit_server = std::stoi(config);

	//[Network]
	config = ConfigManager::readConfig("server_port");
	if (config != "") ConfigManager::server_port = std::stoi(config);
}

void ConfigManager::writeConfig(std::string key, std::string value)
{
	Logger::log("Writing Configuration key: " + key + " value: " + value);
	std::ifstream filein(configFileName); //File to read from
	std::ofstream fileout("_" + configFileName); //Temporary file
	if (!filein || !fileout)
	{
		Logger::log("Error opening configfiles for saving!");
		return;
	}

	char linein[255];
	std::string lineout;
	bool foundKey = false;

	while (!filein.eof())
	{
		std::vector<std::string> param;
		filein.getline(linein, 255);

		if (linein[0] == '\0' ||	//	line is empty
			linein[0] == '#' ||		//	line is comment
			linein[0] == ';' ||		//	line is comment
			linein[0] == '[')		//	line is section
		{
			lineout = linein;
			fileout << lineout;
			if (!filein.eof())
			{
				fileout << '\n';
			}
			continue;
		}

		split(linein, param, '=');

		for (int i = 0; i < param.size(); i++)
		{
			param[i] = trim(param[i]);
		}

		if (param[0] == key)
		{
			lineout = key + " = " + value;
			foundKey = true;
		}
		else
		{
			lineout = linein;
		}

		fileout << lineout;

		if (!filein.eof())
		{
			fileout << '\n';
		}

	}

	if (!foundKey)
	{
		lineout = key + " = " + value;
		fileout << '\n' << lineout;
		Logger::log("While writing did not find this key: " + key + ". Added it at the end");
	}

	filein.close();
	fileout.close();

	std::remove(configFileName.c_str());
	std::rename(("_" + configFileName).c_str(), configFileName.c_str());
}

void ConfigManager::writeAllConfigs()
{
	Logger::log("Writing Configuration");
	//[General]
	ConfigManager::writeConfig("level", ConfigManager::level);
	ConfigManager::writeConfig("bots", std::to_string(ConfigManager::bots));
	ConfigManager::writeConfig("bot_speed_mult", std::to_string(ConfigManager::bot_speed_mult));
	ConfigManager::writeConfig("max_bullets", std::to_string(ConfigManager::max_bullets));
	ConfigManager::writeConfig("fps_limit_server", std::to_string(ConfigManager::fps_limit_server));

	//[Network]
	ConfigManager::writeConfig("server_port", std::to_string(ConfigManager::server_port));

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
	strs.push_back(txt.substr(initialPos, (std::min)(pos, txt.size()) - initialPos + 1));

	return strs.size();
}

std::string ConfigManager::trim(std::string& str)
{
	size_t first = str.find_first_not_of(' ');
	size_t last = str.find_last_not_of(' ');
	return str.substr(first, (last - first + 1));
}