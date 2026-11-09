#pragma once
#include "defines.h"

#ifdef OG_AUDIO_ENABLED

#include <cstdint>
#include <string>
#include <vector>

/// <summary>
/// Live text-to-speech for NPC voices using SVOX Pico TTS.
/// The voicelines are defined in audio/npc/voices.ini, so every line can be
/// changed or added without any audio files.
/// </summary>
class TTSManager
{
public:

	/// One configured voiceline of an event.
	struct Voice
	{
		std::string event;              // event name, e.g. "random" or "hit"
		std::vector<std::string> lines; // all variants of the event
		std::string lang;               // language code, e.g. "de-DE"
		std::string ta;                 // text analysis resource file
		std::string sg;                 // signal generation resource file
		std::string taName;             // resource name as reported by pico
		std::string sgName;
		std::string defName;            // pico voice definition name
		float speed = 1.0f;             // 1.0 = original, higher = faster/higher
	};

	/// A single line ready to be spoken.
	struct Line
	{
		std::string text;
		float speed = 1.0f;
	};

	/// Loads the config, the language resources and initializes the engine.
	/// Returns false if no voice is available (NPCs then stay silent).
	static bool init();

	/// Releases all TTS resources.
	static void shutdown();

	/// True if at least one voiceline is configured and its resources loaded.
	static bool isReady();

	/// All configured voicelines.
	static const std::vector<Voice>& voices();

	/// Randomly picks one line of an event. Returns an empty line if the
	/// event is not configured.
	static Line lineForEvent(const std::string& event);

	/// All configured event names.
	static std::vector<std::string> events();

	/// Reloads audio/npc/voices.ini if the file was modified on disk.
	/// New events and variants are picked up without restarting the game.
	static bool reloadIfChanged();

	/// Synthesizes a text into 16 bit mono pcm samples (16 kHz).
	static std::vector<short> render(const std::string& text, const std::string& lang);

	/// Synthesizes a text with the first configured language.
	static std::vector<short> render(const std::string& text);

	/// Builds an in-memory wav file (mono, 16 bit, little endian) from pcm samples.
	static std::vector<unsigned char> pcmToWav(const std::vector<short>& samples);

private:

	static std::vector<Voice> s_voices;
	static std::int64_t s_configStamp;
};

#endif
