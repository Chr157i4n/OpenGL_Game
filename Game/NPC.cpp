#include "NPC.h"
#include "ConfigManager.h"
#include <string>

NPC::NPC(Shader* shader) : Character(shader)
{
	currentTask = CurrentTask::Idle;
	setType(ObjectType::Object_NPC | ObjectType::Object_Character);
	lookDirection = glm::vec3(1, 0, 1);
	float npc_speed_mult = std::stof(ConfigManager::readConfig("bot_speed_mult"));
	forwardSpeed = forwardSpeed * npc_speed_mult;
}

void NPC::followCharacter(float32 deltaTime, std::vector< std::shared_ptr<Object>> objects, std::shared_ptr <Character> character)
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

	this->moveForward();
}

void NPC::followNavPoints(float32 deltaTime, std::vector< std::shared_ptr<Object>> objects)
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

	this->moveForward();
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

void NPC::doCurrentTask(float32 deltaTime, std::vector< std::shared_ptr<Object>> objects, std::vector< std::shared_ptr<Character>> character)
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

void NPC::evade(float32 deltaTime, CollisionResult collisionResult, std::vector< std::shared_ptr<Object>> objects)
{	
	for (CollidedObject* collidedObject : collisionResult.collidedObjectList)
	{
		if (collidedObject->onTop) continue;

		if (collidedObject->object->getDimensions().y < 3.5)
		{
			jump();
			Logger::log("NPC: " + printObject() + " wants to jump");
		}
		else
		{
			this->moveBackward();
			this->moveRight();
		}

		bool onOtherObjectTemp = this->onOtherObject;
		this->move(deltaTime, objects[1]);
		this->onOtherObject = onOtherObjectTemp;
	}
}