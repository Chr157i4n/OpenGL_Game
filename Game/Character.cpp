#include "Character.h"
#include "Game.h"

Character::Character(Shader* shader) : Object(shader, "character.bmf")
{
	this->shader = shader;
	lookDirection = glm::vec3(1.0f, 0.0f, 0.0f);
	up = glm::vec3(0.0f, 1.0f, 0.0f);
	this->name = "Character";
	this->setCollisionBoxType(CollisionBoxType::cube);
}

glm::vec3 Character::getLookDirection()
{
	return lookDirection;
}

void Character::resetVerticalMovement()
{
	movement = movement * glm::vec3(0, 1, 0);
}

void Character::moveForward() {
	glm::vec3 v = glm::normalize(glm::vec3(1.0f, 0.0f, 1.0f) * getLookDirection()) * forwardSpeed;

	movement.x += v.x;
	movement.z += v.z;
}

void Character::moveBackward() {
	glm::vec3 v = glm::normalize(glm::vec3(1.0f, 0.0f, 1.0f) * getLookDirection()) * -backwardSidewaySpeed;

	movement.x += v.x;
	movement.z += v.z;
}

void Character::moveRight() {
	glm::vec3 v = glm::normalize(glm::vec3(1.0f, 0.0f, 1.0f) * glm::cross(getLookDirection(), up)) * backwardSidewaySpeed;

	movement.x += v.x;
	movement.z += v.z;
}

void Character::moveLeft() {
	glm::vec3 v = glm::normalize(glm::vec3(1.0f, 0.0f, 1.0f) * glm::cross(getLookDirection(), up)) * -backwardSidewaySpeed;

	movement.x += v.x;
	movement.z += v.z;
}

void Character::jump()
{
	if (canJump)
	{
		Logger::log("Character: " + printObject() + " jumped");

		irrklang::vec3df SPosition = irrklang::vec3df(getPosition().x, getPosition().y, getPosition().z);
		irrklang::ISound* jumpSound = Game::getSoundEngine()->play3D("audio/jump.wav", SPosition, false, false, true);

		//jumpSound->setPosition(SPosition);

		movement.y = 0.5;
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

Object* Character::getObjectLookingAt()
{
	return nullptr;
}

/// <summary>
/// creates a bullet at the position of the character, with a little offset.
//  it gives the bullet also the direction the character is looking at.
/// </summary>
/// <returns>the created bullet</returns>
std::shared_ptr<Bullet> Character::shoot()
{
	if (Game::getTimestamp() > lastTimeShot+1)
	{
		glm::vec3 bulletCreationPosition = position + glm::vec3(0, 3, 0);
		std::shared_ptr<Bullet> newBullet = std::make_shared<Bullet>(shader, bulletCreationPosition, glm::vec3(0, rotation.y - 90, -rotation.x - 90), getLookDirection());

		Game::bullets.push_back(newBullet);
		Game::objects.push_back(newBullet);


		lastTimeShot = Game::getTimestamp();

		return newBullet;
	}
	return nullptr;
}



