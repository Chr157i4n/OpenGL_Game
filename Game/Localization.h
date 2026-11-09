#pragma once
#include "defines.h"

#include <string>
#include <unordered_map>

// Localizable UI text.
//
// All menu strings are read from locale/<language>.json (key: value object) so
// labels can be changed without recompiling. Keys that are missing in the file
// fall back to the built-in German defaults, so the game still runs when the
// locale file is absent.
class Locale
{
public:
	// language is taken from ConfigManager::language. If that locale file is
	// missing, "de" is loaded as fallback.
	static void init(std::string language = "");

	static std::string get(const std::string& key);

	static bool has(const std::string& key);

	static std::string language();

private:
	static bool loadFile(const std::string& path);

	static std::unordered_map<std::string, std::string> entries;
	static std::unordered_map<std::string, std::string> defaults;
	static std::string currentLanguage;
};
