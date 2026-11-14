#include "Object.h"

#include "Game.h"
#include "Map.h"
#include "ResourceManager.h"

static glm::vec3 sanitizedDimensions(glm::vec3 dimensions)
{
	if (glm::length(dimensions) < 0.1f)
	{
		return glm::vec3(2, 2, 2);
	}

	return dimensions;
}

Object::Object(std::string modelFileName)
	: ObjectCore(glm::vec3(2, 2, 2))
{
	this->model = ResourceManager::getModelByName(modelFileName);

	if (this->model)
	{
		setDimensions(sanitizedDimensions(this->model->getDimension()));
	}
}

CollisionResult Object::checkCollision()
{
	std::vector<std::shared_ptr<ObjectCore>> world;
	world.reserve(Game::objects.size() + Game::clients.size());

	for (const std::shared_ptr<Object>& object : Game::objects)
	{
		world.push_back(std::static_pointer_cast<ObjectCore>(object));
	}

	for (const std::shared_ptr<Client>& client : Game::clients)
	{
		world.push_back(std::static_pointer_cast<ObjectCore>(client));
	}

	return ObjectCore::checkCollision(world);
}

void Object::reactToCollision(CollisionResult collisionResult)
{
	ObjectCore::reactToCollision(collisionResult, Game::getDelta());
}

bool Object::checkBoundaries()
{
	if (Game::map.empty())
	{
		return false;
	}

	return ObjectCore::checkBoundaries(std::static_pointer_cast<ObjectCore>(Game::map[0]), Game::getDelta());
}

void Object::fall()
{
	ObjectCore::fall(Game::getDelta(), Map::getGravity());
}

void Object::move()
{
	std::shared_ptr<ObjectCore> map = Game::map.empty() ? nullptr : std::static_pointer_cast<ObjectCore>(Game::map[0]);

	ObjectCore::move(Game::getDelta(), map);
}

float Object::getDistance(std::shared_ptr<Object> object) const
{
	return ObjectCore::getDistance(std::static_pointer_cast<ObjectCore>(object));
}

void Object::setModel(Model* newModel)
{
	model = newModel;

	if (model)
	{
		setDimensions(sanitizedDimensions(model->getDimension()));
	}
	else
	{
		setDimensions(glm::vec3(2, 2, 2));
	}
}

void Object::setModel(std::string newModelFileName)
{
	setModel(ResourceManager::getModelByName(newModelFileName));
}

Model* Object::getModel() const
{
	return model;
}