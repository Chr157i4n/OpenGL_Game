#include "Object.h"

#include "Game.h"
#include "Logger.h"
#include "Map.h"
#include "Raypicker.h"
#include "Renderer.h"
#include "ResourceManager.h"

static glm::vec3 sanitizedDimensions(glm::vec3 dimensions)
{
	if (glm::length(dimensions) < 0.1f)
	{
		return glm::vec3(2, 2, 2);
	}

	return dimensions;
}

Object::Object(Shader* shader, std::string modelFileName)
	: ObjectCore(glm::vec3(2, 2, 2))
{
	this->shader = shader;
	this->model = ResourceManager::getModelByName(modelFileName);

	if (this->model)
	{
		setDimensions(sanitizedDimensions(this->model->getDimension()));
	}

	markDuration = 10;
}

Shader* Object::getShader()
{
	return shader;
}

void Object::bindShader()
{
	if (shader)
	{
		shader->bind();
	}
}

void Object::unbindShader()
{
	if (shader)
	{
		shader->unbind();
	}
}

bool Object::intersectWithRay(Ray ray) const
{
	return ObjectCore::intersectWithRay(ray.origin, ray.direction);
}

float Object::getDistance(std::shared_ptr<Object> object) const
{
	return ObjectCore::getDistance(std::static_pointer_cast<ObjectCore>(object));
}

float Object::getDistance(glm::vec3 location) const
{
	return std::abs(glm::length(location - this->getCenter()));
}

void Object::interact_click()
{
	if (!this->getEnabled())
		return;
}

void Object::interact_hold()
{
	if (!this->getEnabled())
		return;
}

void Object::reactToCollision(CollisionResult collisionResult)
{
	ObjectCore::reactToCollision(collisionResult, Game::getDelta());
}

CollisionResult Object::checkCollision()
{
	std::vector<std::shared_ptr<ObjectCore>> world;
	world.reserve(Game::objects.size() + Game::clients.size());

	for (const std::shared_ptr<Object>& object : Game::objects)
	{
		if (object)
		{
			world.push_back(std::static_pointer_cast<ObjectCore>(object));
		}
	}

	for (const std::shared_ptr<Character>& client : Game::clients)
	{
		if (client)
		{
			world.push_back(std::static_pointer_cast<ObjectCore>(client));
		}
	}

	return ObjectCore::checkCollision(world);
}

bool Object::checkBoundaries()
{
	if (Game::map.empty())
		return false;

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

void Object::render()
{
	if (!model)
		return;

	glm::vec2 currentTextureFlowPosition = textureFlow * glm::vec2(Game::getTimeStamp() * 0.001, Game::getTimeStamp() * 0.001);
	int textureFlowUniformLocation = GLCALL(glGetUniformLocation(Renderer::getShader(ShaderType::basic)->getShaderId(), "u_textureflow"));
	GLCALL(glUniform2fv(textureFlowUniformLocation, 1, &currentTextureFlowPosition[0]));

	model->render();
}

void Object::renderShadowMap()
{
	if (model)
		model->renderShadowMap();
}

void Object::renderEnvMap()
{
	if (model)
		model->renderEnvMap();
}

void Object::updateCollisionGeometry()
{
	ObjectCore::updateCollisionGeometry();

	orientedBoundingBox.minX = std::numeric_limits<float>::infinity();
	orientedBoundingBox.maxX = -std::numeric_limits<float>::infinity();
	orientedBoundingBox.minY = std::numeric_limits<float>::infinity();
	orientedBoundingBox.maxY = -std::numeric_limits<float>::infinity();
	orientedBoundingBox.minZ = std::numeric_limits<float>::infinity();
	orientedBoundingBox.maxZ = -std::numeric_limits<float>::infinity();

	for (const glm::vec3& point : cubePoints)
	{
		if (point.x < orientedBoundingBox.minX)
			orientedBoundingBox.minX = point.x;
		if (point.x > orientedBoundingBox.maxX)
			orientedBoundingBox.maxX = point.x;
		if (point.y < orientedBoundingBox.minY)
			orientedBoundingBox.minY = point.y;
		if (point.y > orientedBoundingBox.maxY)
			orientedBoundingBox.maxY = point.y;
		if (point.z < orientedBoundingBox.minZ)
			orientedBoundingBox.minZ = point.z;
		if (point.z > orientedBoundingBox.maxZ)
			orientedBoundingBox.maxZ = point.z;
	}
}
