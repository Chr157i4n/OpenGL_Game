#include "NPC.h"

#include "Game.h"
#include "ConfigManager.h"

NPC::NPC(Shader* shader) : Character(shader)
{
	currentTask = CurrentTask::Idle;
	setType(ObjectType::Object_NPC | ObjectType::Object_Character);
	lookDirection = glm::vec3(1, 0, 1);
	float npc_speed_mult = ConfigManager::bot_speed_mult;
	forwardSpeed = forwardSpeed * npc_speed_mult;
	backwardSidewaySpeed = backwardSidewaySpeed * npc_speed_mult;

	std::string path = "audio\\npc";
	if (std::filesystem::exists(path))
	{
		for (const auto& entry : std::filesystem::directory_iterator(path))
		{
			if(entry.path().string().find("random") != std::string::npos)
				voicefiles.push_back(entry.path().string());
		}
	}

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
		followCharacter(Game::players[0]);
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
			#ifdef DEBUG_MOVEMENT
			Logger::log("NPC: " + printObject() + " wants to jump");
			#endif
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
		if (character->getTeam() == this->getTeam() && this->getTeam()!=0) continue;

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

void NPC::interact_click()
{
	if (!this->getEnabled()) return;

	int randomvoicefile = (std::rand() * voicefiles.size() / RAND_MAX);

	this->speak(voicefiles[randomvoicefile]);
}

void NPC::move()
{
	this->Object::move();

	if (movement != glm::vec3(0, 0, 0) && voice != nullptr)
	{
		voice->setPosition(AudioManager::glmVec3toIrrklang(this->getCenter()));
	}
}

void NPC::registerHit()
{
	this->Object::registerHit();
	
	this->speak("audio\\npc\\npc_hit.mp3");
}

void NPC::speak(std::string audioFile)
{
	if (voice == nullptr)
	{
		voice = AudioManager::play3D(audioFile, this->getCenter(), AudioType::voice);
	}
	else if (voice->isFinished())
	{
		voice->drop();
		voice = AudioManager::play3D(audioFile, this->getCenter(), AudioType::voice);
	}
}
