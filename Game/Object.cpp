#include "Object.h"

Object::Object(Shader* shader, std::string modelFileName)
{
	setObjectType(ObjectType::Object_Entity);

	position = glm::vec3(0, 0, 0);
	rotation = glm::vec3(0, 0, 0);
	movement = glm::vec3(0, 0, 0);
	dimensions = glm::vec3(2, 2, 2);

	this->shader = shader;

	model = new Model;
	loadModel(modelFileName);
}

bool Object::checkCollision(Object* object, glm::vec3 newPosition)
{
	// X - Achse
	if (newPosition.x + dimensions.x / 2 < object->getPosition().x - object->getDimensions().x / 2)
		return false;

	if (newPosition.x - dimensions.x / 2 > object->getPosition().x + object->getDimensions().x / 2)
		return false;

	// Y - Achse
	if (newPosition.y + dimensions.y / 2< object->getPosition().y - object->getDimensions().y / 2)
		return false;

	if (newPosition.y - dimensions.y / 2 > object->getPosition().y + object->getDimensions().y / 2)
		return false;

	// Z - Achse
	if (newPosition.z + dimensions.z / 2 < object->getPosition().z - object->getDimensions().z / 2)
		return false;

	if (newPosition.z - dimensions.z / 2 > object->getPosition().z + object->getDimensions().z / 2)
		return false;

	std::cout << "Collision with: " << object->getName() << " Objecttype: " << object->getObjectType() << std::endl;
	return true;
}

bool Object::checkBoundaries(Object* map, glm::vec3 newPosition)
{
	// X - Achse
	if (newPosition.x - dimensions.x / 2 < map->getPosition().x - map->getDimensions().x / 2)
	{
		std::cout << "Player out of bounds. Obejctposition: x: " << position.x << " y: " << position.y << " z: " << position.z << std::endl;
		return true;
	}

	if (newPosition.x + dimensions.x / 2 > map->getPosition().x + map->getDimensions().x / 2)
	{
		std::cout << "Player out of bounds. Obejctposition: x: " << position.x << " y: " << position.y << " z: " << position.z << std::endl;
		return true;
	}

	/*
	// Y - Achse
	if (newPosition.y - dimensions.y / 2 < map->getPosition().y - map->getDimensions().y / 2)
	{
		std::cout << "Player out of bounds. Obejctposition: x: " << position.x << " y: " << position.y << " z: " << position.z << std::endl;
		return true;
	}

	if (newPosition.y + dimensions.y / 2 > map->getPosition().y + map->getDimensions().y / 2)
	{
		std::cout << "Player out of bounds. Obejctposition: x: " << position.x << " y: " << position.y << " z: " << position.z << std::endl;
		return true;
	}
	*/

	// Z - Achse
	if (newPosition.z - dimensions.z / 2 < map->getPosition().z - map->getDimensions().z / 2)
	{
		std::cout << "Player out of bounds. Obejctposition: x: " << position.x << " y: " << position.y << " z: " << position.z << std::endl;
		return true;
	}

	if (newPosition.z + dimensions.z / 2 > map->getPosition().z + map->getDimensions().z / 2)
	{
		std::cout << "Player out of bounds. Obejctposition: x: " << position.x << " y: " << position.y << " z: " << position.z << std::endl;
		return true;
	}
	
	
	return false;
}

//deltaTime in seconds
void Object::fall(float32 deltaTime)
{
	if (position.y > 0)
	{
		movement.y -= 9.81 * 200 * deltaTime;
	}

	if (position.y <= 0)
	{
		movement.y = 0;
		position.y = 0;
	}
}

//deltaTime in seconds
void Object::move(float32 deltaTime)
{
	position += movement;
}

void Object::loadModel(std::string modelFileName)
{
	const char* fileNameChar = modelFileName.c_str();
	name = modelFileName;
	model->init(fileNameChar, shader);
}

Model* Object::getModel()
{
	return model;
}

void Object::setPosition(glm::vec3 newPosition)
{
	position = newPosition;
}

glm::vec3 Object::getPosition()
{
	return position;
}

void Object::setRotation(glm::vec3 newRotation)
{
	rotation = newRotation;
}

glm::vec3 Object::getRotation()
{
	return rotation;
}

void Object::setMovement(glm::vec3 newMovement)
{
	movement = newMovement;
}

glm::vec3 Object::getMovement()
{
	return movement;
}

void Object::calculateDimensions()
{
	dimensions = model->getDimension();
	std::cout << "Object Dimension" << " x: " << dimensions.x << " y: " << dimensions.y << " z: " << dimensions.z << std::endl;
}

glm::vec3 Object::getDimensions()
{
	return dimensions;
}

void Object::setObjectType(ObjectType newObjectType)
{
	objectType = newObjectType;
}

ObjectType Object::getObjectType()
{
	return objectType;
}

std::string Object::getName()
{
	return name;
}

void Object::render()
{
	this->model->render();
}
