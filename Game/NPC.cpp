#include "NPC.h"
#include "ConfigManager.h"
#include <string>

NPC::NPC(Shader* shader, float fov, float width, float height) : Character(shader)
{
	setType(ObjectType::Object_Bot);
	lookDirection = glm::vec3(1, 0, 1);
	float npc_speed_mult = std::stof(ConfigManager::readConfig("bot_speed_mult"));
	forwardSpeed = forwardSpeed * npc_speed_mult;
}

void NPC::followCharacter(float32 deltaTime, std::vector<Object*> objects, Character* character)
{
	glm::vec3 myPosition = position;
	glm::vec3 targetPosition = character->getPosition();

	glm::vec3 direction = targetPosition - myPosition;

	lookDirection.x = direction.x;
	lookDirection.z = direction.z;

	lookDirection = glm::normalize(lookDirection);

	float pitch = glm::degrees(asin(-lookDirection.y));
	float yaw = glm::degrees(atan2(lookDirection.x, lookDirection.z));
	setRotation(glm::vec3(0, yaw, 0));

	this->moveForward(objects);
}
