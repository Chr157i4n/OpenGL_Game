#pragma once

#include <irrKlang.h>
#include "libs/glm/glm.hpp"

#include <string>

static class AudioManager
{
public:

	static void init();

	static void deinit();

	static void setVolume();

	static irrklang::vec3df glmVec3toIrrklang(glm::vec3 vector);

	static irrklang::ISound* play2D(std::string musicFile, bool playLooped = false);

	static irrklang::ISound* play3D(std::string musicFile, glm::vec3 position, bool playLooped = false);

	static void updateAudioListener();


private:
	static irrklang::ISoundEngine* SoundEngine;

	static irrklang::ISound* music;
};

