#include "Character.h"

Character::Character(Shader* shader) : Object(shader, "models/character.bmf")
{
	this->shader = shader;
	lookDirection = glm::vec3(1.0f, 0.0f, 0.0f);
}

void Character::moveForward() {
	glm::vec3 v = glm::normalize(glm::vec3(1.0f, 0.0f, 1.0f) * lookDirection) * forwardSpeed;
	position += v;
}

void Character::moveBackward() {
	glm::vec3 v = glm::normalize(glm::vec3(1.0f, 0.0f, 1.0f) * lookDirection) * -backwardSidewaySpeed;
	position += v;
}

void Character::moveRight() {
	glm::vec3 v = glm::normalize(glm::vec3(1.0f, 0.0f, 1.0f) * lookDirection) * backwardSidewaySpeed;
	position += v;
}

void Character::moveLeft() {
	glm::vec3 v = glm::normalize(glm::vec3(1.0f, 0.0f, 1.0f) * lookDirection) * -backwardSidewaySpeed;
	position += v;
}

void Character::jump()
{
	movement.y = 0.5;
}

//deltaTime in seconds
void Character::fall(float32 deltaTime) 
{
	position.y += upwardSpeed;

	if (position.y > 0)
	{
		upwardSpeed -= 9.81 * 200 * deltaTime;
	}

	if (position.y <= 0)
	{
		upwardSpeed = 0;
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


