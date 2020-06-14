#include "Character.h"

Character::Character(Shader* shader) : Object(shader, "models/character.bmf")
{
	this->shader = shader;
	lookDirection = glm::vec3(1.0f, 0.0f, 0.0f);
	up = glm::vec3(0.0f, 1.0f, 0.0f);
}

void Character::moveForward(std::vector<Object*> objects) {
	glm::vec3 v = glm::normalize(glm::vec3(1.0f, 0.0f, 1.0f) * lookDirection) * forwardSpeed;

	for (Object* object : objects)
	{
		if (object->getNumber() == this->getNumber()) continue;
		if (object->getType() == ObjectType::Object_Environment) continue;
		if (object->getType() == ObjectType::Object_Bullet) continue;
		if (checkCollisionXY(object, position + v))
		{
			moveBackward(objects);
			moveRight(objects);
			return;
		}
	}
	if (checkBoundaries(objects[1], position + v)) return;

	position += v;
}

void Character::moveBackward(std::vector<Object*> objects) {
	glm::vec3 v = glm::normalize(glm::vec3(1.0f, 0.0f, 1.0f) * lookDirection) * -backwardSidewaySpeed;

	for (Object* object : objects)
	{
		if (object->getNumber() == this->getNumber()) continue;
		if (object->getType() == ObjectType::Object_Environment) continue;
		if (object->getType() == ObjectType::Object_Bullet) continue;
		if (checkCollisionXY(object, position + v)) return;
	}
	if (checkBoundaries(objects[1], position + v)) return;

	position += v;
}

void Character::moveRight(std::vector<Object*> objects) {
	glm::vec3 v = glm::normalize(glm::vec3(1.0f, 0.0f, 1.0f) * glm::cross(lookDirection, up)) * backwardSidewaySpeed;

	for (Object* object : objects)
	{
		if (object->getNumber() == this->getNumber()) continue;
		if (object->getType() == ObjectType::Object_Environment) continue;
		if (object->getType() == ObjectType::Object_Bullet) continue;
		if (checkCollisionXY(object, position + v)) return;
	}
	if (checkBoundaries(objects[1], position + v)) return;

	position += v;
}

void Character::moveLeft(std::vector<Object*> objects) {
	glm::vec3 v = glm::normalize(glm::vec3(1.0f, 0.0f, 1.0f) * glm::cross(lookDirection, up)) * -backwardSidewaySpeed;

	for (Object* object : objects)
	{
		if (object->getNumber() == this->getNumber()) continue;
		if (object->getType() == ObjectType::Object_Environment) continue;
		if (object->getType() == ObjectType::Object_Bullet) continue;
		if (checkCollisionXY(object, position + v)) return;
	}
	if (checkBoundaries(objects[1], position + v)) return;

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


