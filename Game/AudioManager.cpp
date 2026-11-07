#include "AudioManager.h"

#include "ConfigManager.h"
#include "Game.h"
#include "Logger.h"
#include "Player.h"

#include <algorithm>
#include <memory>
#include <string>
#include <unordered_map>

#ifdef OG_AUDIO_ENABLED
#include <soloud.h>
#include <soloud_wav.h>
#endif

namespace
{
#ifdef OG_AUDIO_ENABLED
	SoLoud::Soloud* g_engine = nullptr;
	std::unordered_map<std::string, std::unique_ptr<SoLoud::Wav>> g_soundSources;
	std::shared_ptr<AudioHandle> g_music;

	std::string normalizePath(std::string path)
	{
		std::replace(path.begin(), path.end(), '\\', '/');
		return path;
	}

	SoLoud::Wav* loadSoundSource(const std::string& fileName)
	{
		if (!g_engine) return nullptr;

		const std::string path = normalizePath(fileName);
		auto existing = g_soundSources.find(path);
		if (existing != g_soundSources.end())
		{
			return existing->second.get();
		}

		auto source = std::make_unique<SoLoud::Wav>();
		SoLoud::result result = source->load(path.c_str());
		if (result != SoLoud::SO_NO_ERROR)
		{
			Logger::log("Error loading sound '" + path + "': " + g_engine->getErrorString(result));
			return nullptr;
		}

		SoLoud::Wav* sourcePtr = source.get();
		g_soundSources.emplace(path, std::move(source));
		return sourcePtr;
	}

	float volumeForType(AudioType type)
	{
		switch (type)
		{
		case AudioType::music:       return ConfigManager::music_volume;
		case AudioType::ambient:     return ConfigManager::ambient_volume;
		case AudioType::voice:       return ConfigManager::voice_volume;
		case AudioType::soundeffect:
		default:                     return ConfigManager::effect_volume;
		}
	}
#endif
}

void AudioManager::init()
{
#ifdef OG_AUDIO_ENABLED
	g_engine = new SoLoud::Soloud();

	SoLoud::result result = g_engine->init(
		SoLoud::Soloud::CLIP_ROUNDOFF | SoLoud::Soloud::LEFT_HANDED_3D,
		SoLoud::Soloud::SDL2,
		48000,
		2048,
		2);

	if (result != SoLoud::SO_NO_ERROR)
	{
		Logger::log(std::string("Error while starting SoLoud SoundEngine: ") + g_engine->getErrorString(result));
		delete g_engine;
		g_engine = nullptr;
		return;
	}

	g_engine->set3dListenerParameters(0, 0, 0, 0, 0, 1, 0, 1, 0);

	g_music = play2D("audio/breakout.mp3", AudioType::music, true);
	setVolume();
#else
	Logger::log("Audio support is disabled at build time.");
#endif
}

void AudioManager::deinit()
{
#ifdef OG_AUDIO_ENABLED
	g_music.reset();

	if (g_engine)
	{
		g_engine->stopAll();
		g_soundSources.clear();
		g_engine->deinit();
		delete g_engine;
		g_engine = nullptr;
	}
#endif
}

void AudioManager::setVolume()
{
#ifdef OG_AUDIO_ENABLED
	if (!g_engine) return;

	g_engine->setGlobalVolume(std::clamp(ConfigManager::master_volume, 0.0f, 1.0f));

	if (g_music)
	{
		g_music->setVolume(ConfigManager::music_volume);
	}
#endif
}

std::shared_ptr<AudioHandle> AudioManager::play2D(const std::string& musicFile, AudioType audiotype, bool playLooped)
{
#ifdef OG_AUDIO_ENABLED
	if (!g_engine) return nullptr;

	SoLoud::Wav* source = loadSoundSource(musicFile);
	if (!source) return nullptr;

	SoLoud::handle handle = g_engine->play(*source, volumeForType(audiotype));
	if (handle == 0)
	{
		Logger::log("Error playing sound: " + musicFile);
		return nullptr;
	}

	g_engine->setLooping(handle, playLooped);
	return std::make_shared<AudioHandle>(handle);
#else
	(void)musicFile;
	(void)audiotype;
	(void)playLooped;
	return nullptr;
#endif
}

std::shared_ptr<AudioHandle> AudioManager::play3D(const std::string& musicFile, const glm::vec3& position, AudioType audiotype, bool playLooped)
{
#ifdef OG_AUDIO_ENABLED
	if (!g_engine) return nullptr;

	SoLoud::Wav* source = loadSoundSource(musicFile);
	if (!source) return nullptr;

	SoLoud::handle handle = g_engine->play3d(
		*source,
		position.x, position.y, position.z,
		0.0f, 0.0f, 0.0f,
		volumeForType(audiotype));

	if (handle == 0)
	{
		Logger::log("Error playing sound: " + musicFile);
		return nullptr;
	}

	g_engine->setLooping(handle, playLooped);
	return std::make_shared<AudioHandle>(handle);
#else
	(void)musicFile;
	(void)position;
	(void)audiotype;
	(void)playLooped;
	return nullptr;
#endif
}

void AudioManager::updateAudioListener()
{
#ifdef OG_AUDIO_ENABLED
	if (!g_engine || Game::players.empty() || !Game::players[0]) return;

	glm::vec3 position = Game::players[0]->getPosition();
	glm::vec3 lookAt = Game::players[0]->getLookDirection();

	// Keep the same coordinate conversion the game used with irrKlang.
	lookAt.x = -lookAt.x;
	lookAt.z = -lookAt.z;

	g_engine->set3dListenerParameters(
		position.x, position.y, position.z,
		lookAt.x, lookAt.y, lookAt.z,
		0.0f, 1.0f, 0.0f);
#endif
}

void AudioHandle::setVolume(float volume) const
{
#ifdef OG_AUDIO_ENABLED
	if (!g_engine || voice == 0) return;
	g_engine->setVolume(voice, std::clamp(volume, 0.0f, 1.0f));
#else
	(void)volume;
#endif
}

void AudioHandle::setPosition(const glm::vec3& position) const
{
#ifdef OG_AUDIO_ENABLED
	if (!g_engine || voice == 0) return;
	g_engine->set3dSourcePosition(voice, position.x, position.y, position.z);
#else
	(void)position;
#endif
}

bool AudioHandle::isFinished() const
{
#ifdef OG_AUDIO_ENABLED
	return !g_engine || voice == 0 || !g_engine->isValidVoiceHandle(voice);
#else
	return true;
#endif
}

void AudioHandle::stop() const
{
#ifdef OG_AUDIO_ENABLED
	if (g_engine && voice != 0)
	{
		g_engine->stop(voice);
	}
#endif
}

void AudioHandle::drop()
{
	stop();
	voice = 0;
}

bool AudioHandle::isValid() const
{
	return !isFinished();
}
