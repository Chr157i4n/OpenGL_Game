#include "TTSManager.h"

#ifdef OG_AUDIO_ENABLED

#include "picoapi.h"

#include "Logger.h"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <random>
#include <string>
#include <vector>

std::vector<TTSManager::Voice> TTSManager::s_voices;
std::int64_t TTSManager::s_configStamp = 0;

namespace
{
	const pico_Uint32 PICO_MEM_SIZE = 2500000;
	const pico_Int16 PICO_OUTBUF_SIZE = 128;

	pico_System g_system = nullptr;
	std::vector<unsigned char> g_picoMemory;

	const char* const VOICE_CONFIG_FILE = "audio/npc/voices.ini";

	// Counter for pico voice definition names. It only grows, so a reload can
	// create definitions for newly configured languages without clashing with
	// definitions that are still in use.
	int s_nextVoiceDef = 0;

	std::int64_t fileStamp(const std::string& path)
	{
		std::error_code ec;
		std::filesystem::file_time_type time = std::filesystem::last_write_time(path, ec);
		if (ec) return 0;
		return static_cast<std::int64_t>(time.time_since_epoch().count());
	}

	std::string trim(const std::string& value)
	{
		const std::string whitespace = " \t\r\n";
		size_t start = value.find_first_not_of(whitespace);
		if (start == std::string::npos) return std::string();
		size_t end = value.find_last_not_of(whitespace);
		return value.substr(start, end - start + 1);
	}

	std::string lower(const std::string& value)
	{
		std::string result = value;
		for (char& c : result)
		{
			c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
		}
		return result;
	}

	bool fileExists(const std::string& path)
	{
		std::error_code ec;
		return std::filesystem::exists(path, ec);
	}

	// Look for a pico language resource in the npc audio folder first,
	// then in the pico source tree (useful for a fresh checkout).
	std::string findResource(const std::string& fileName)
	{
		const std::string candidates[] = {
			"audio/npc/" + fileName,
			"pico/lang/" + fileName,
		};

		for (const std::string& candidate : candidates)
		{
			if (fileExists(candidate)) return candidate;
		}

		return std::string();
	}

	std::vector<std::string> splitVariants(const std::string& text)
	{
		std::vector<std::string> variants;
		std::string current;

		for (char c : text)
		{
			if (c == '|')
			{
				std::string variant = trim(current);
				if (!variant.empty()) variants.push_back(variant);
				current.clear();
			}
			else
			{
				current.push_back(c);
			}
		}

		std::string variant = trim(current);
		if (!variant.empty()) variants.push_back(variant);

		return variants;
	}

	void parseOptions(const std::string& options, TTSManager::Voice& voice)
	{
		size_t pos = 0;
		while (pos < options.size())
		{
			size_t next = options.find(';', pos);
			std::string pair = trim(options.substr(pos, next == std::string::npos ? std::string::npos : next - pos));
			pos = next == std::string::npos ? options.size() : next + 1;

			size_t separator = pair.find('=');
			if (separator == std::string::npos) continue;

			std::string key = lower(trim(pair.substr(0, separator)));
			std::string value = trim(pair.substr(separator + 1));

			if (key == "speed")
			{
				voice.speed = std::clamp(std::stof(value), 0.1f, 2.0f);
			}
			else if (key == "lang" || key == "language")
			{
				voice.lang = lower(value);
			}
			else if (key == "ta")
			{
				voice.ta = value;
			}
			else if (key == "sg")
			{
				voice.sg = value;
			}
		}
	}

	bool loadConfig(std::vector<TTSManager::Voice>& voices)
	{
		const std::string configFile = VOICE_CONFIG_FILE;
		if (!fileExists(configFile))
		{
			Logger::log("TTS: missing voice config " + configFile);
			return false;
		}

		std::ifstream file(configFile);
		if (!file)
		{
			Logger::log("TTS: could not open " + configFile);
			return false;
		}

		std::string defaultLang = "de-DE";
		float defaultSpeed = 1.0f;

		std::string line;
		while (std::getline(file, line))
		{
			line = trim(line);
			if (line.empty() || line[0] == '#' || line[0] == ';') continue;
			if (line[0] == '[') continue;

			size_t separator = line.find('=');
			if (separator == std::string::npos) continue;

			std::string key = lower(trim(line.substr(0, separator)));
			std::string value = trim(line.substr(separator + 1));

			if (key == "sprache" || key == "language" || key == "lang")
			{
				defaultLang = lower(value);
				continue;
			}

			if (key == "speed")
			{
				defaultSpeed = std::clamp(std::stof(value), 0.1f, 2.0f);
				continue;
			}

			// Everything after ';' is an optional parameter list.
			std::string text = value;
			std::string options;
			size_t optionStart = text.find(';');
			if (optionStart != std::string::npos)
			{
				options = text.substr(optionStart + 1);
				text = text.substr(0, optionStart);
			}

			std::vector<std::string> lines = splitVariants(text);
			if (lines.empty()) continue;

			TTSManager::Voice voice;
			voice.event = key;
			voice.lines = lines;
			voice.lang = defaultLang;
			voice.speed = defaultSpeed;

			parseOptions(options, voice);

			if (voice.ta.empty()) voice.ta = findResource(voice.lang + "_ta.bin");
			if (voice.sg.empty())
			{
				const std::string guesses[] = {
					voice.lang + "_gl0_sg.bin",
					voice.lang + "_lh0_sg.bin",
					voice.lang + "_kh0_sg.bin",
					voice.lang + "_sg.bin",
				};

				for (const std::string& guess : guesses)
				{
					voice.sg = findResource(guess);
					if (!voice.sg.empty()) break;
				}
			}

			if (voice.ta.empty() || voice.sg.empty())
			{
				Logger::log("TTS: no pico language files found for '" + voice.lang + "' (voiceline '" + key + "')");
				continue;
			}

			voices.push_back(voice);
		}

		return !voices.empty();
	}

	// Creates a pico voice definition (language resources + definition) for a
	// single voiceline entry.
	bool createVoiceDefinition(TTSManager::Voice& voice)
	{
		pico_Resource taResource = nullptr;
		pico_Resource sgResource = nullptr;

		pico_Status status = pico_loadResource(g_system, reinterpret_cast<const pico_Char *>(voice.ta.c_str()), &taResource);
		if (status != PICO_OK)
		{
			pico_Retstring message;
			pico_getSystemStatusMessage(g_system, status, message);
			Logger::log("TTS: could not load " + voice.ta + ": " + std::string(message));
			return false;
		}

		status = pico_loadResource(g_system, reinterpret_cast<const pico_Char *>(voice.sg.c_str()), &sgResource);
		if (status != PICO_OK)
		{
			pico_Retstring message;
			pico_getSystemStatusMessage(g_system, status, message);
			Logger::log("TTS: could not load " + voice.sg + ": " + std::string(message));
			pico_unloadResource(g_system, &taResource);
			return false;
		}

		pico_Retstring taName;
		pico_Retstring sgName;
		pico_getResourceName(g_system, taResource, taName);
		pico_getResourceName(g_system, sgResource, sgName);

		const std::string defName = "voice_" + std::to_string(s_nextVoiceDef++);
		status = pico_createVoiceDefinition(g_system, reinterpret_cast<const pico_Char *>(defName.c_str()));
		if (status != PICO_OK)
		{
			pico_Retstring message;
			pico_getSystemStatusMessage(g_system, status, message);
			Logger::log("TTS: could not create voice definition: " + std::string(message));
			pico_unloadResource(g_system, &sgResource);
			pico_unloadResource(g_system, &taResource);
			return false;
		}

		pico_addResourceToVoiceDefinition(g_system, reinterpret_cast<const pico_Char *>(defName.c_str()), reinterpret_cast<const pico_Char *>(taName));
		pico_addResourceToVoiceDefinition(g_system, reinterpret_cast<const pico_Char *>(defName.c_str()), reinterpret_cast<const pico_Char *>(sgName));

		voice.taName = taName;
		voice.sgName = sgName;
		voice.defName = defName;
		return true;
	}

	bool loadLanguageResources(std::vector<TTSManager::Voice>& voices)
	{
		for (size_t i = 0; i < voices.size(); ++i)
		{
			TTSManager::Voice& voice = voices[i];

			// Voices with the same language share their resources.
			for (const TTSManager::Voice& other : voices)
			{
				if (&other == &voice) continue;
				if (other.defName.empty()) continue;
				if (other.ta == voice.ta && other.sg == voice.sg)
				{
					voice.taName = other.taName;
					voice.sgName = other.sgName;
					voice.defName = other.defName;
					break;
				}
			}

			if (!voice.defName.empty()) continue;

			if (!createVoiceDefinition(voice)) return false;
		}

		return true;
	}
}

bool TTSManager::init()
{
	s_voices.clear();

	if (!loadConfig(s_voices))
	{
		Logger::log("TTS: no voicelines configured, NPC voices are disabled");
		return false;
	}

	g_picoMemory.assign(PICO_MEM_SIZE, 0);

	pico_Status status = pico_initialize(g_picoMemory.data(), PICO_MEM_SIZE, &g_system);
	if (status != PICO_OK || g_system == nullptr)
	{
		pico_Retstring message;
		if (g_system) pico_getSystemStatusMessage(g_system, status, message);
		Logger::log("TTS: could not initialize pico engine: " + std::string(g_system ? message : "no memory"));
		g_system = nullptr;
		s_voices.clear();
		return false;
	}

	if (!loadLanguageResources(s_voices))
	{
		pico_terminate(&g_system);
		g_system = nullptr;
		s_voices.clear();
		return false;
	}

	s_configStamp = fileStamp(VOICE_CONFIG_FILE);

	Logger::log("TTS: ready, " + std::to_string(s_voices.size()) + " voicelines configured");
	return true;
}

bool TTSManager::reloadIfChanged()
{
	if (g_system == nullptr) return false;

	const std::int64_t stamp = fileStamp(VOICE_CONFIG_FILE);
	if (stamp == 0 || stamp == s_configStamp) return false;

	s_configStamp = stamp;

	std::vector<Voice> fresh;
	if (!loadConfig(fresh) || fresh.empty())
	{
		Logger::log("TTS: could not reload " + std::string(VOICE_CONFIG_FILE) + ", keeping the current voicelines");
		return false;
	}

	for (Voice& voice : fresh)
	{
		// Languages that are already loaded keep their voice definition.
		for (const Voice& existing : s_voices)
		{
			if (existing.defName.empty()) continue;
			if (existing.ta == voice.ta && existing.sg == voice.sg)
			{
				voice.taName = existing.taName;
				voice.sgName = existing.sgName;
				voice.defName = existing.defName;
				break;
			}
		}

		if (voice.defName.empty() && !createVoiceDefinition(voice))
		{
			Logger::log("TTS: could not reload " + std::string(VOICE_CONFIG_FILE) + ", keeping the current voicelines");
			return false;
		}
	}

	s_voices.swap(fresh);

	Logger::log("TTS: reloaded " + std::to_string(s_voices.size()) + " voicelines from " + std::string(VOICE_CONFIG_FILE));
	return true;
}

void TTSManager::shutdown()
{
	for (const Voice& voice : s_voices)
	{
		if (!voice.defName.empty())
		{
			pico_releaseVoiceDefinition(g_system, reinterpret_cast<const pico_Char *>(voice.defName.c_str()));
		}
	}

	if (g_system != nullptr)
	{
		pico_terminate(&g_system);
		g_system = nullptr;
	}

	g_picoMemory.clear();
	s_voices.clear();
}

bool TTSManager::isReady()
{
	return g_system != nullptr && !s_voices.empty();
}

const std::vector<TTSManager::Voice>& TTSManager::voices()
{
	return s_voices;
}

std::vector<std::string> TTSManager::events()
{
	std::vector<std::string> result;
	for (const Voice& voice : s_voices)
	{
		result.push_back(voice.event);
	}
	return result;
}

TTSManager::Line TTSManager::lineForEvent(const std::string& event)
{
	Line line;

	reloadIfChanged();

	if (s_voices.empty()) return line;

	const std::string needle = lower(event);

	std::vector<const Voice*> matches;
	for (const Voice& voice : s_voices)
	{
		if (voice.event == needle) matches.push_back(&voice);
	}

	if (matches.empty()) return line;

	static std::mt19937 generator(std::random_device{}());

	const Voice* voice = matches[0];
	if (matches.size() > 1)
	{
		std::uniform_int_distribution<size_t> pick(0, matches.size() - 1);
		voice = matches[pick(generator)];
	}

	if (voice->lines.empty()) return line;

	if (voice->lines.size() > 1)
	{
		std::uniform_int_distribution<size_t> pick(0, voice->lines.size() - 1);
		line.text = voice->lines[pick(generator)];
	}
	else
	{
		line.text = voice->lines[0];
	}

	line.speed = voice->speed;

	return line;
}

std::vector<short> TTSManager::render(const std::string& text)
{
	if (s_voices.empty()) return std::vector<short>();
	return render(text, s_voices.front().lang);
}

std::vector<short> TTSManager::render(const std::string& text, const std::string& lang)
{
	std::vector<short> samples;

	if (g_system == nullptr || text.empty()) return samples;

	const std::string needle = lower(lang);

	const Voice* voice = nullptr;
	for (const Voice& candidate : s_voices)
	{
		if (candidate.lang == needle)
		{
			voice = &candidate;
			break;
		}
	}

	if (voice == nullptr) voice = &s_voices.front();

	static pico_Engine s_engine = nullptr;
	static std::string s_engineDef;
	if (s_engineDef != voice->defName)
	{
		if (s_engine != nullptr)
		{
			pico_disposeEngine(g_system, &s_engine);
			s_engine = nullptr;
		}
		pico_Status engineStatus = pico_newEngine(g_system, reinterpret_cast<const pico_Char *>(voice->defName.c_str()), &s_engine);
		if (engineStatus != PICO_OK || s_engine == nullptr)
		{
			pico_Retstring message;
			pico_getSystemStatusMessage(g_system, engineStatus, message);
			Logger::log("TTS: could not create engine: " + std::string(message));
			return samples;
		}
		s_engineDef = voice->defName;
	}

	pico_Engine engine = s_engine;
	pico_Status status = PICO_OK;

	// The terminating NUL has to be part of the text: it tells the engine that
	// the input is complete so it flushes its buffers and actually synthesizes
	// short lines (see picotts pico/bin/pico2wave.c: strlen(text) + 1).
	const pico_Int16 textLength = static_cast<pico_Int16>(text.size() + 1);
	pico_Int16 bytesPut = 0;
	status = pico_putTextUtf8(engine, reinterpret_cast<const pico_Char *>(text.c_str()), textLength, &bytesPut);

	// The synthesis loop has to call pico_getData at least once (do-while),
	// even when pico_putTextUtf8 already returned PICO_OK.
	unsigned char outbuf[PICO_OUTBUF_SIZE];
	pico_Int16 bytesReceived = 0;
	pico_Int16 dataType = 0;

	pico_Status getStatus = PICO_OK;
	do {
		getStatus = pico_getData(engine, outbuf, PICO_OUTBUF_SIZE, &bytesReceived, &dataType);
		if (getStatus == PICO_STEP_BUSY || getStatus == PICO_STEP_IDLE)
		{
			const short* pcm = reinterpret_cast<const short*>(outbuf);
			for (pico_Int16 i = 0; i < bytesReceived / 2; ++i)
			{
				samples.push_back(pcm[i]);
			}
		}
	} while (getStatus == PICO_STEP_BUSY);
	status = getStatus;

	if (status != PICO_STEP_IDLE)
	{
		pico_Retstring message;
		pico_getEngineStatusMessage(engine, status, message);
		Logger::log("TTS: could not speak '" + text + "': " + std::string(message));
		samples.clear();
	}

	return samples;
}

std::vector<unsigned char> TTSManager::pcmToWav(const std::vector<short>& samples)
{
	const unsigned int dataSize = static_cast<unsigned int>(samples.size() * sizeof(short));
	std::vector<unsigned char> wav(44 + dataSize, 0);

	auto writeUInt32 = [&](size_t offset, unsigned int value)
	{
		wav[offset] = static_cast<unsigned char>(value & 0xFF);
		wav[offset + 1] = static_cast<unsigned char>((value >> 8) & 0xFF);
		wav[offset + 2] = static_cast<unsigned char>((value >> 16) & 0xFF);
		wav[offset + 3] = static_cast<unsigned char>((value >> 24) & 0xFF);
	};

	writeUInt32(0, 0x46464952); // "RIFF"
	writeUInt32(4, 36 + dataSize);
	writeUInt32(8, 0x45564157); // "WAVE"
	writeUInt32(12, 0x20746d66); // "fmt "
	writeUInt32(16, 16);
	wav[20] = 1;  // PCM
	wav[22] = 1;  // mono
	wav[24] = 16; // 16 bit
	writeUInt32(26, 16000);     // sample rate
	writeUInt32(30, 32000);     // bytes per second
	wav[34] = 2;                // block align
	writeUInt32(36, dataSize);
	writeUInt32(40, 0x61746164); // "data"

	std::memcpy(wav.data() + 44, samples.data(), dataSize);

	return wav;
}

#endif
