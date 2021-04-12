#include "Character.h"

#include "Game.h"
#include "Raypicker.h"

#include <cmath>

Character::Character(Shader* shader) : Object(shader, "character.bmf")
{
	this->shader = shader;
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
			objectPlayerLookingAt->interact_click();
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
#ifdef DEBUG_MOVEMENT
		Logger::log("Character: " + printObject() + " jumped");
#endif

		AudioManager::play3D("audio/jump.wav", this->getPosition());

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

std::shared_ptr<Object> Character::calculateObjectLookingAt()
{
	glm::vec3 rayOrigin = this->getLookOrigin();
	glm::vec3 rayDirection = this->getLookDirection();

	RayPickingResult raypickingresult = Raypicker::doRayPicking(Ray(rayOrigin, rayDirection), Game::objects);

#ifdef DEBUG_RAYPICKING
	Logger::log(this->printObject()+" is looking at "+ raypickingresult.intersectedObject_nearest.object->printObject());
#endif
	objectLookingAt = raypickingresult.intersectedObject_nearest.object;
	return raypickingresult.intersectedObject_nearest.object;
}

/// <summary>
/// creates a bullet at the position of the character, with a little offset.
///  it gives the bullet also the direction the character is looking at.
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
		glm::vec3 bulletCreationPosition = position + glm::vec3(0, 2, 0) +glm::vec3(1, 1, 1) * getLookDirection();
		glm::vec3 bulletCreationRotation = glm::vec3(0, rotation.y - 90 + scatteringAngleY, -rotation.x - 90 + scatteringAngleX);
		glm::vec3 bulletCreationDirection = getLookDirection();
		
		//rotate direction around Y
		bulletCreationDirection = glm::rotate(bulletCreationDirection, glm::radians(scatteringAngleY), glm::vec3(0, 1, 0));

		//rotate direction around z
		bulletCreationDirection = glm::rotate(bulletCreationDirection, glm::radians(scatteringAngleX), glm::vec3(0, 0, 1));


		std::shared_ptr<Bullet> newBullet = std::make_shared<Bullet>(shader, bulletCreationPosition, bulletCreationRotation, bulletCreationDirection);
		newBullet->setNumber(Game::objects.size());

		Game::bullets.push_back(newBullet);
		Game::objects.push_back(newBullet);
		newBullet->calculationBeforeFrame();


		lastTimeShot = std::chrono::system_clock::now();

		AudioManager::play3D("audio/shoot.wav", this->getPosition());

		return newBullet;
	}
	return nullptr;
}



