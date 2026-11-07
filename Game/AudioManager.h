#pragma once

#include <glm/glm.hpp>

#include <memory>
#include <string>

enum AudioType {
	soundeffect,
	voice,
	ambient,
	music,
};

/// <summary>
/// Lightweight handle for a sound started by AudioManager.  The handle is valid
/// until the sound finishes or drop() is called.  It intentionally hides the
/// audio backend (SoLoud) from the rest of the game.
/// </summary>
class AudioHandle
{
public:
	AudioHandle() = default;
	explicit AudioHandle(unsigned int voiceHandle) : voice(voiceHandle) {}

	void setVolume(float volume) const;
	void setPosition(const glm::vec3& position) const;
	bool isFinished() const;
	void stop() const;
	void drop();
	bool isValid() const;

private:
	unsigned int voice = 0;
};


class AudioManager
{
public:

	static void init();

	static void deinit();

	static void setVolume();

	static std::shared_ptr<AudioHandle> play2D(const std::string& musicFile, AudioType audiotype = AudioType::soundeffect, bool playLooped = false);

	static std::shared_ptr<AudioHandle> play3D(const std::string& musicFile, const glm::vec3& position, AudioType audiotype = AudioType::soundeffect, bool playLooped = false);

	static void updateAudioListener();
};
