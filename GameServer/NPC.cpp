#include "NPC.h"

#include "Game.h"
#include "ConfigManager.h"

NPC::NPC() : Character()
{
	currentTask = CurrentTask::Idle;
	setType(ObjectType::Object_NPC | ObjectType::Object_Character);
	lookDirection = glm::vec3(1, 0, 1);
	float npc_speed_mult = ConfigManager::bot_speed_mult;
	forwardSpeed = forwardSpeed * npc_speed_mult;
	backwardSidewaySpeed = backwardSidewaySpeed * npc_speed_mult;
}

void NPC::followCharacter(std::shared_ptr <Character> character)
{
	glm::vec3 myPosition = position;
	glm::vec3 targetPosition = character->getPosition();

	lookDirection = targetPosition - myPosition;		//aim at the target

	float distance = glm::length(this->getPosition() - character->getPosition());
	float maxDistance = 30;

	if (distance < maxDistance)
	{
		lookDirection.y += (0.00015 * Game::getDelta() * pow(distance,2));		//aim a little bit higher, when the enemy is more far away
	}

	lookDirection = glm::normalize(lookDirection);

	float pitch = glm::degrees(asin(-lookDirection.y));
	float yaw = glm::degrees(atan2(lookDirection.x, lookDirection.z));
	setRotation(glm::vec3(0, yaw, 0));

	if (distance < maxDistance)
	{
		this->shoot();
	}

	if (distance > 10)
	{
		this->moveForward();
	}
}

void NPC::followNavPoints()
{
	
	glm::vec3 myPosition = position;
	glm::vec3 targetPosition = navPoints[currentNavPoint];

	lookDirection = targetPosition - myPosition;
	lookDirection = glm::normalize(lookDirection);

	float pitch = glm::degrees(asin(-lookDirection.y));
	float yaw = glm::degrees(atan2(lookDirection.x, lookDirection.z));
	setRotation(glm::vec3(0, yaw, 0));

	float distance = glm::length(targetPosition - myPosition);

	if (distance < 0.5)
	{
		currentNavPoint = (currentNavPoint + 1) % navPoints.size();
	}
	else
	{
		this->moveForward();
	}
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

void NPC::doCurrentTask()
{
	switch (currentTask)
	{
	case CurrentTask::Idle:

		break;
	case CurrentTask::Follow_Character:
		followCharacter(Game::clients[0]);
		break;
	case CurrentTask::Follow_NavPoint:
		followNavPoints();
		break;
	case CurrentTask::Attack:
		attackCurrentTarget();
		break;
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

void NPC::evade(CollisionResult collisionResult)
{	
#ifdef DEBUG_NPC
	Logger::log("npc needs to evade an object");
#endif

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
		this->move();
		this->onOtherObject = onOtherObjectTemp;
	}
}

void NPC::reactToCollision(CollisionResult collisionResult)
{
	this->Object::reactToCollision(collisionResult);
}

void NPC::calculateNextTarget()
{
	std::shared_ptr<Character> nextTarget = nullptr;
	float nextTargetdistance= (std::numeric_limits<float>::max)();

	for (std::shared_ptr<Character> character : Game::characters)
	{
		if (!character->getEnabled()) continue;
		if (character->getType() & ObjectType::Object_Player) continue;
		if (character->getNumber() == this->getNumber()) continue;
		if (character->getTeam() == this->getTeam()) continue;

		float distance = glm::length( character->getPosition() - this->getPosition() );

		if (distance < nextTargetdistance)
		{
			nextTargetdistance = distance;
			nextTarget = character;
		}
	}

	currentTarget = nextTarget;
}

void NPC::attackCurrentTarget()
{
	if (currentTarget != nullptr)
	{
		if (currentTarget->isAlive() && currentTarget->getEnabled())
		{
			followCharacter(currentTarget);
			return;
		}
	}

	calculateNextTarget();

}