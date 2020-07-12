#include "NPC.h"
#include "ConfigManager.h"
#include <string>

NPC::NPC(Shader* shader) : Character(shader)
{
	currentTask = CurrentTask::Idle;
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

void NPC::followNavPoints(float32 deltaTime, std::vector<Object*> objects)
{
	if (round(position) == round(navPoints[currentNavPoint]))
	{
		currentNavPoint = (currentNavPoint + 1) % navPoints.size();
	}
	
	glm::vec3 myPosition = position;
	glm::vec3 targetPosition = navPoints[currentNavPoint];

	glm::vec3 direction = targetPosition - myPosition;

	lookDirection.x = direction.x;
	lookDirection.z = direction.z;

	lookDirection = glm::normalize(lookDirection);

	float pitch = glm::degrees(asin(-lookDirection.y));
	float yaw = glm::degrees(atan2(lookDirection.x, lookDirection.z));
	setRotation(glm::vec3(0, yaw, 0));

	this->moveForward(objects);
}

void NPC::addNavPoint(glm::vec3 newNavPoint)
{
	navPoints.push_back(newNavPoint);
}

void NPC::setNavPoints(std::vector<glm::vec3> newNavPoints)
{
	navPoints = newNavPoints;
}

std::vector<glm::vec3> NPC::getNavPoints()
{
	return navPoints;
}

void NPC::doCurrentTask(float32 deltaTime, std::vector<Object*> objects, std::vector<Character*> character)
{
	if (currentTask == CurrentTask::Idle)
	{

	}
	if (currentTask == CurrentTask::Follow_Character)
	{
		followCharacter(deltaTime, objects, character[0]);
	}
	if (currentTask == CurrentTask::Follow_NavPoint)
	{
		followNavPoints(deltaTime, objects);
	}
}

void NPC::setCurrentTask(CurrentTask newCurrentTask)
{
	currentTask = newCurrentTask;
}

CurrentTask NPC::getGurrentTask()
{
	return currentTask;
}
