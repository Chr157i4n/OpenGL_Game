#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <algorithm>
#include "Logger.h"


class ConfigManager
{

public:

	ConfigManager(std::string nConfigFileName)
	{
		configFileName = nConfigFileName;
	}

	std::string readConfig(std::string key)
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

private:

	size_t split(const std::string& txt, std::vector<std::string>& strs, char ch)
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

	std::string trim(std::string& str)
	{
		size_t first = str.find_first_not_of(' ');
		size_t last = str.find_last_not_of(' ');
		return str.substr(first, (last - first + 1));
	}

	std::string configFileName;

};

