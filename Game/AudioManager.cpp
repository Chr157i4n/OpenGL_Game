#include "AudioManager.h"

#include "ConfigManager.h"
#include "Player.h"
#include "Game.h"

irrklang::ISoundEngine* AudioManager::SoundEngine;
irrklang::ISound* AudioManager::music;

void AudioManager::init()
{
	AudioManager::SoundEngine = irrklang::createIrrKlangDevice();

	if (!SoundEngine)
	{
		Logger::log("Error while starting irrKlang SoundEngine");
		return; // error starting up the engine
	}


	music = play2D("audio/breakout.mp3", true);

	AudioManager::setVolume();
}

void AudioManager::deinit()
{
	music->drop();
	SoundEngine->drop();
}

void AudioManager::setVolume()
{
	AudioManager::SoundEngine->setSoundVolume(ConfigManager::master_volume);
	music->setVolume(ConfigManager::music_volume);
}


irrklang::vec3df AudioManager::glmVec3toIrrklang(glm::vec3 vector)
{
	return irrklang::vec3df(vector.x, vector.y, vector.z);
}

irrklang::ISound* AudioManager::play2D(std::string musicFile, bool playLooped)
{
	irrklang::ISound* sound = SoundEngine->play2D(musicFile.c_str(), playLooped, false, true);
	if (!sound)
	{
		Logger::log("Error playing sound: " + musicFile);
		return nullptr;
	}
	sound->setVolume(ConfigManager::music_volume);

	return sound;
}

irrklang::ISound* AudioManager::play3D(std::string musicFile, glm::vec3 position, bool playLooped)
{
	irrklang::ISound* sound = SoundEngine->play3D(musicFile.c_str(), glmVec3toIrrklang(position), playLooped, false, true);
	if (!sound)
	{
		Logger::log("Error playing sound: " + musicFile);
		return nullptr;
	}
	
	sound->setVolume(ConfigManager::effect_volume);

	return sound;
}

void AudioManager::updateAudioListener()
{
	irrklang::vec3df position = glmVec3toIrrklang(Game::players[0]->getPosition());
	irrklang::vec3df lookAt = glmVec3toIrrklang(Game::players[0]->getLookDirection());

	lookAt.X = -lookAt.X;
	lookAt.Z = -lookAt.Z;

	SoundEngine->setListenerPosition(position, lookAt);
}
