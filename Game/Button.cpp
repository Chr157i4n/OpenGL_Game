#include "Button.h"

#include "LuaManager.h"
#include "AudioManager.h"


Button::Button(Shader* shader, std::string modelFileName) : Object(shader, modelFileName)
{
	this->setType(ObjectType::Object_Interactable | ObjectType::Object_Entity);
}

void Button::interact_click()
{
	if (!this->getEnabled()) return;
	if (interactLuaFunction_click == "") return;

	std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
	std::chrono::duration<double, std::milli> animDuration = now - lastInteractTimestamp;
	if (animDuration.count() < this->travel_duration) return;

	this->markObject();
	LuaManager::runFunction(interactLuaFunction_click);

	lastInteractTimestamp = std::chrono::system_clock::now();
	originalPosition = this->getPosition();
	AudioManager::play3D("audio/interact.wav", this->getPosition());
}

void Button::interact_hold()
{
	if (!this->getEnabled()) return;
	if (interactLuaFunction_hold == "") return;

	this->markObject();
	LuaManager::runFunction(interactLuaFunction_hold);
}

void Button::move()
{
	std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
	std::chrono::duration<double, std::milli> animDuration = now - lastInteractTimestamp;
	float animDurationCount = animDuration.count();
	
	if (animDuration.count() < this->travel_duration)
	{

		if (animDuration.count() < this->travel_duration * 1/3)
		{ // move
			movement = travel*2.0f / (travel_duration/1000);
		}
		else if (animDuration.count() > this->travel_duration * 2/3)
		{ // move back
			movement = -(travel*2.0f / (travel_duration/1000));
		}
		else
		{
			movement = glm::vec3(0);
		}

	}
	else if(movement != glm::vec3(0))
	{
		this->setPosition(originalPosition);
		movement = glm::vec3(0);
	}
	
	this->Object::move();
}
