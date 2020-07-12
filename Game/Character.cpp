#include "Character.h"

Character::Character(Shader* shader) : Object(shader, "models/character.bmf")
{
	this->shader = shader;
	lookDirection = glm::vec3(1.0f, 0.0f, 0.0f);
	up = glm::vec3(0.0f, 1.0f, 0.0f);
}

glm::vec3 Character::getLookDirection()
{
	return lookDirection;
}

void Character::resetVerticalMovement()
{
	movement = movement * glm::vec3(0, 1, 0);
}

void Character::moveForward(std::vector<Object*> objects) {
	glm::vec3 v = glm::normalize(glm::vec3(1.0f, 0.0f, 1.0f) * getLookDirection()) * forwardSpeed;

	if (checkBoundaries(objects[1], position + v)) return;

	movement.x += v.x;
	movement.z += v.z;
}

void Character::moveBackward(std::vector<Object*> objects) {
	glm::vec3 v = glm::normalize(glm::vec3(1.0f, 0.0f, 1.0f) * getLookDirection()) * -backwardSidewaySpeed;

	if (checkBoundaries(objects[1], position + v)) return;

	movement.x += v.x;
	movement.z += v.z;
}

void Character::moveRight(std::vector<Object*> objects) {
	glm::vec3 v = glm::normalize(glm::vec3(1.0f, 0.0f, 1.0f) * glm::cross(getLookDirection(), up)) * backwardSidewaySpeed;

	if (checkBoundaries(objects[1], position + v)) return;

	movement.x += v.x;
	movement.z += v.z;
}

void Character::moveLeft(std::vector<Object*> objects) {
	glm::vec3 v = glm::normalize(glm::vec3(1.0f, 0.0f, 1.0f) * glm::cross(getLookDirection(), up)) * -backwardSidewaySpeed;

	if (checkBoundaries(objects[1], position + v)) return;

	movement.x += v.x;
	movement.z += v.z;
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


