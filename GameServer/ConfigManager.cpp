#include "ConfigManager.h"

void ConfigManager::init(std::string nConfigFileName)
{
	ConfigManagerBase::loadConfigFile(nConfigFileName);
	ConfigManager::readAllConfigs();
}

void ConfigManager::readAllConfigs()
{
	Logger::log("Reading Configuration");
	ConfigManagerBase::readCommonConfigs();
}

void ConfigManager::writeAllConfigs()
{
	Logger::log("Writing Configuration");
	ConfigManagerBase::writeCommonConfigs();
}
