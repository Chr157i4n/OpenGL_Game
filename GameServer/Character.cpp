#include "Character.h"
#include "Game.h"

#include <cmath>

Character::Character() : Object("character.bmf")
{
	lookDirection = glm::vec3(1.0f, 0.0f, 0.0f);
	up = glm::vec3(0.0f, 1.0f, 0.0f);
	this->name = "Character";
	this->setType(ObjectType::Object_Character);
	this->setCollisionBoxType(CollisionBoxType::cube);
}

glm::vec3 Character::getLookDirection()
{
	return lookDirection;
}

glm::vec3 Character::getLookOrigin()
{
	glm::vec3 lookOrigin = this->getPosition();
	lookOrigin.y += this->getDimensions().y * 0.75;
	return lookOrigin;
}

void Character::interactWithObject()
{
	std::shared_ptr<Object> objectPlayerLookingAt = this->getObjectLookingAt();
	if (objectPlayerLookingAt != nullptr)
	{
		if (this->getDistance(objectPlayerLookingAt) < 10)
		{
			objectPlayerLookingAt->interact();
		}
	}
}

void Character::resetVerticalMovement()
{
	movement = movement * glm::vec3(0, 1, 0);
}

void Character::moveForward() {
	glm::vec3 v = glm::normalize(glm::vec3(1.0f, 0.0f, 1.0f) * getLookDirection()) * forwardSpeed;// *Game::getDelta() / 1000.0f;

	movement.x += v.x;
	movement.z += v.z;
}

void Character::moveBackward() {
	glm::vec3 v = glm::normalize(glm::vec3(1.0f, 0.0f, 1.0f) * getLookDirection()) * -backwardSidewaySpeed;// *Game::getDelta() / 1000.0f;

	movement.x += v.x;
	movement.z += v.z;
}

void Character::moveRight() {
	glm::vec3 v = glm::normalize(glm::vec3(1.0f, 0.0f, 1.0f) * glm::cross(getLookDirection(), up)) * backwardSidewaySpeed;// *Game::getDelta() / 1000.0f;

	movement.x += v.x;
	movement.z += v.z;
}

void Character::moveLeft() {
	glm::vec3 v = glm::normalize(glm::vec3(1.0f, 0.0f, 1.0f) * glm::cross(getLookDirection(), up)) * -backwardSidewaySpeed; //* Game::getDelta() / 1000.0f;

	movement.x += v.x;
	movement.z += v.z;
}

void Character::jump()
{
	if (canJump)
	{
		Logger::log("Character: " + printObject() + " jumped");

		movement.y += 30;
		//movement.y += 0.06 * Game::getDelta();

		canJump = false;
	}
}

void Character::activateJumping()
{
	if (!canJump)
	{
		canJump = true;
		Logger::log("Jumping activated for: " + printObject());
	}
}

void Character::crouch(bool crouch)
{
	if (crouch && !isCrouched)
	{
		isCrouched = true;
	}
	else if (!crouch && isCrouched)
	{
		isCrouched = false;
	}
}

void Character::run(bool run)
{
	if (run && !isRunning)
	{
		forwardSpeed = forwardSpeed * 2;
		isRunning = true;
	}
	else if (!run && isRunning)
	{
		forwardSpeed = forwardSpeed / 2;
		isRunning = false;
	}
}

std::shared_ptr<Object> Character::getObjectLookingAt()
{
	std::vector< std::shared_ptr<Object> > objectsLookingAt;

	for (std::shared_ptr<Object> object : Game::objects)
	{
		if (object->getType() & ObjectType::Object_Player) continue;
		if (object->getType() & ObjectType::Object_Environment) continue;
		if (!object->getEnabled()) continue;

		glm::vec3 rayOrigin = this->getLookOrigin();

		glm::vec3 rayDirection = this->getLookDirection();

		if (object->intersectWithRay(rayOrigin, rayDirection))
		{
			objectsLookingAt.push_back(object);
		}
	}

	//return the object with the minimum distance
	float minDistance = (std::numeric_limits<float>::max)();
	float currentDistance = 0;
	std::shared_ptr<Object> objectMinDistance = nullptr;

	for (std::shared_ptr<Object> object : objectsLookingAt)
	{
		currentDistance = this->getDistance(object);
		if (currentDistance < minDistance)
		{
			minDistance = currentDistance;
			objectMinDistance = object;
		}
	}

	return objectMinDistance;
}

/// <summary>
/// creates a bullet at the position of the character, with a little offset.
//  it gives the bullet also the direction the character is looking at.
/// </summary>
/// <returns>a shared pointer to the bullet</returns>
std::shared_ptr<Bullet> Character::shoot()
{
	float maxScatteringAngle = 1;

	float scatteringAngleX = std::rand() * maxScatteringAngle / RAND_MAX;
	scatteringAngleX -= maxScatteringAngle / 2;

	float scatteringAngleY = std::rand() * maxScatteringAngle / RAND_MAX;
	scatteringAngleY -= maxScatteringAngle / 2;

	std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
	std::chrono::duration<double, std::milli> notshotDuration = now - lastTimeShot;
	if (notshotDuration.count() > 1000)
	{
		glm::vec3 bulletCreationPosition = position + glm::vec3(0, 2, 0) + glm::vec3(1, 1, 1) * getLookDirection();
		glm::vec3 bulletCreationRotation = glm::vec3(0, rotation.y - 90 + scatteringAngleY, -rotation.x - 90 + scatteringAngleX);
		glm::vec3 bulletCreationDirection = getLookDirection();

		//rotate direction around Y
		bulletCreationDirection = glm::rotate(bulletCreationDirection, glm::radians(scatteringAngleY), glm::vec3(0, 1, 0));

		//rotate direction around z
		bulletCreationDirection = glm::rotate(bulletCreationDirection, glm::radians(scatteringAngleX), glm::vec3(0, 0, 1));


		std::shared_ptr<Bullet> newBullet = std::make_shared<Bullet>(bulletCreationPosition, bulletCreationRotation, bulletCreationDirection);
		newBullet->setNumber(Game::objects.size());
		newBullet->setNetworkID(Game::objects[Game::objects.size()-1]->getNetworkID()+1);

		Game::bullets.push_back(newBullet);
		Game::objects.push_back(newBullet);


		lastTimeShot = std::chrono::system_clock::now();

		return newBullet;
	}
	return nullptr;
}




