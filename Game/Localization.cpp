#include "Localization.h"

#include <fstream>

#include <nlohmann/json.hpp>

#include "Helper.h"
#include "Logger.h"

std::unordered_map<std::string, std::string> Locale::entries;
std::unordered_map<std::string, std::string> Locale::defaults;
std::string Locale::currentLanguage = "de";

// Built in fallbacks, so a missing locale file never leaves the UI empty.
static void fillDefaults(std::unordered_map<std::string, std::string>& map)
{
	map["menu.singleplayer"]        = "Einzelspieler";
	map["menu.multiplayer"]         = "Mehrspieler";
	map["menu.mapeditor"]           = "Mapeditor";
	map["menu.options"]             = "Optionen";
	map["menu.exit"]                = "Beenden";
	map["menu.startmap"]            = "Map starten";
	map["menu.playername"]          = "Spielername";
	map["menu.noMaps"]              = "Keine Maps gefunden";

	map["menu.connect"]             = "Verbinden";
	map["menu.back"]                = "Zurueck";
	map["menu.serveraddress"]       = "Server Adresse";
	map["menu.serverport"]          = "Server Port";

	map["menu.resume"]              = "Weiterspielen";
	map["menu.restart"]             = "Neustart";
	map["menu.mainmenu"]            = "Hauptmenu";

	map["menu.editorstart"]         = "Starten";
	map["menu.mapname"]             = "Map Name";

	map["menu.fullscreen"]          = "Vollbild";
	map["menu.fullscreen.on"]       = "Vollbild";
	map["menu.fullscreen.off"]      = "Fenster";
	map["menu.fullscreen.borderless"] = "Randloses Fenster";

	map["menu.vsync"]               = "V_Sync: Aus";
	map["menu.vsync.off"]           = "V_Sync: Aus";
	map["menu.vsync.on"]            = "V_Sync: An";

	map["menu.shadows"]             = "Schatten: Weich";
	map["menu.shadows.off"]         = "Schatten: Aus";
	map["menu.shadows.hard"]        = "Schatten: Hart";
	map["menu.shadows.soft"]        = "Schatten: Weich";

	map["menu.volume.master"]       = "Master-Volume";
	map["menu.volume.music"]        = "Musik-Volume";
	map["menu.volume.effect"]       = "Effekt-Volume";
	map["menu.volume.ambient"]      = "Umgebungs-Volume";
	map["menu.volume.voice"]        = "Stimmen-Volume";

	map["menu.shadowresolution"]    = "Schatten-Aufloesung";
	map["menu.reflectionresolution"] = "Reflektions-Aufloesung";
	map["menu.resolution"]          = "Aufloesung";

	map["menu.hint"]                = "W/S: ausgewaehlt   A/D: Wert andern   Enter: bestaetigen";
}

bool Locale::loadFile(const std::string& path)
{
	std::ifstream file(path);
	if (!file)
		return false;

	nlohmann::json json;
	try
	{
		json = nlohmann::json::parse(file);
	}
	catch (const std::exception& e)
	{
		Logger::log("Could not parse locale file " + path + ": " + e.what());
		return false;
	}

	int loaded = 0;
	for (const auto& entry : json.items())
	{
        if (entry.value().is_string())
        {
            entries[entry.key()] = entry.value().get<std::string>();
            loaded++;
        }
	}

	Logger::log("Loaded locale " + path + " (" + std::to_string(loaded) + " keys)");
	return true;
}

void Locale::init(std::string language)
{
	if (defaults.empty())
		fillDefaults(defaults);

	if (language.empty())
		language = "de";

	currentLanguage = language;

	if (!loadFile("locale/" + language + ".json"))
	{
		Logger::log("Locale file for '" + language + "' not found, falling back to 'de'");
		if (!loadFile("locale/de.json"))
		{
            Logger::log("No locale file available, using built in defaults");
        }
		currentLanguage = "de";
	}
}

std::string Locale::get(const std::string& key)
{
	auto it = entries.find(key);
	if (it != entries.end())
		return it->second;

	auto fallback = defaults.find(key);
	if (fallback != defaults.end())
		return fallback->second;

	return key;
}

bool Locale::has(const std::string& key)
{
	return entries.find(key) != entries.end() || defaults.find(key) != defaults.end();
}

std::string Locale::language()
{
	return currentLanguage;
}
