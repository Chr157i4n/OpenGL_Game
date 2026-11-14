#include "ObjectCore.h"

#include "Logger.h"

#include <utility>

static float projectedOverlap(float minA, float maxA, float minB, float maxB)
{
	return (std::max)(0.0f, (std::min)(maxA, maxB) - (std::max)(minA, minB));
}

ObjectCore::ObjectCore(glm::vec3 initialDimensions)
{
	setType(ObjectType::Object_Entity);

	position = glm::vec3(0, 0, 0);
	rotation = glm::vec3(0, 0, 0);
	movement = glm::vec3(0, 0, 0);
	scale = glm::vec3(1, 1, 1);

	setDimensions(initialDimensions);

	this->name = "Object";

	this->setGravity(true);
}

bool ObjectCore::intersectWithRay(glm::vec3 rayOrigin, glm::vec3 rayDirection) const
{
	const float epsilon = 1e-8f;

	float minX = position.x - dimensions.x / 2;
	float maxX = position.x + dimensions.x / 2;

	float minY = position.y;
	float maxY = position.y + dimensions.y;

	float minZ = position.z - dimensions.z / 2;
	float maxZ = position.z + dimensions.z / 2;

	float tMin = 0.0f;
	float tMax = std::numeric_limits<float>::infinity();

	auto slab = [&](float axisMin, float axisMax, float origin, float direction) -> bool
	{
		if (std::abs(direction) < epsilon)
		{
			return origin >= axisMin - epsilon && origin <= axisMax + epsilon;
		}

		float t1 = (axisMin - origin) / direction;
		float t2 = (axisMax - origin) / direction;

		if (t1 > t2)
		{
			std::swap(t1, t2);
		}

		if (t1 > tMin) tMin = t1;
		if (t2 < tMax) tMax = t2;

		return tMin <= tMax;
	};

	if (!slab(minX, maxX, rayOrigin.x, rayDirection.x)) return false;
	if (!slab(minY, maxY, rayOrigin.y, rayDirection.y)) return false;
	if (!slab(minZ, maxZ, rayOrigin.z, rayDirection.z)) return false;

	return glm::dot(rayDirection, glm::vec3((minX + maxX) / 2, (minY + maxY) / 2, (minZ + maxZ) / 2) - rayOrigin) > 0.0f;
}

float ObjectCore::getDistance(std::shared_ptr<ObjectCore> other) const
{
	if (!other) return 0.0f;

	return std::abs(glm::length(other->getPosition() - this->getPosition()));
}

void ObjectCore::markObject()
{
	lastMarkTimestamp = std::chrono::system_clock::now();
}

void ObjectCore::markObject(float32 durationMs)
{
	markDuration = durationMs;
	lastMarkTimestamp = std::chrono::system_clock::now();
}

bool ObjectCore::isMarked() const
{
	std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
	std::chrono::duration<double, std::milli> markDurationElapsed = now - lastMarkTimestamp;

	return markDurationElapsed.count() <= markDuration;
}

void ObjectCore::interact()
{
	registerHit();
}

CollisionResult ObjectCore::checkCollision(const std::vector<std::shared_ptr<ObjectCore>>& world)
{
	CollisionResult collisionResult;
	collisionResult.collided = false;

	if (this->movement == glm::vec3(0, 0, 0)) return collisionResult;

	ObjectCore* self = this;

	for (const std::shared_ptr<ObjectCore>& other : world)
	{
		if (!other || other.get() == self) continue;
		if (!other->getEnabled()) continue;
		if (other->getCollisionBoxType() == CollisionBoxType::none) continue;

		// Bullets move faster than everything else, so only bullets should
		// check collision against other objects.
		if (other->getType() == ObjectType::Object_Bullet) continue;

		if (checkCollision_AABB(other))
		{
			checkCollision_SAT(other, &collisionResult);
		}
	}

	return collisionResult;
}

bool ObjectCore::checkCollision_AABB(std::shared_ptr<ObjectCore> other) const
{
	if (!other) return false;

	glm::vec3 object2Position = other->getPosition();

	float left1 = position.x - boundingboxdimensions.x / 2;
	float right1 = position.x + boundingboxdimensions.x / 2;
	float top1 = position.y + dimensions.y;
	float bottom1 = position.y;
	float front1 = position.z - boundingboxdimensions.z / 2;
	float back1 = position.z + boundingboxdimensions.z / 2;

	float left2 = object2Position.x - other->getBoundingBoxDimensions().x / 2;
	float right2 = object2Position.x + other->getBoundingBoxDimensions().x / 2;
	float top2 = object2Position.y + other->getDimensions().y;
	float bottom2 = object2Position.y;
	float front2 = object2Position.z - other->getBoundingBoxDimensions().z / 2;
	float back2 = object2Position.z + other->getBoundingBoxDimensions().z / 2;

	bool xOverlap = (left1 <= right2 && left2 <= right1);
	bool yOverlap = (bottom1 <= top2 && bottom2 <= top1);
	bool zOverlap = (front1 <= back2 && front2 <= back1);

	return xOverlap && yOverlap && zOverlap;
}

bool ObjectCore::checkCollision_SAT(std::shared_ptr<ObjectCore> other, CollisionResult* collisionResult)
{
	if (!other || !collisionResult) return false;

	std::vector<std::vector<glm::vec3>> collisionPoints;
	float minOverlap = std::numeric_limits<float>::infinity();
	glm::vec3 minOverlapAxis = glm::vec3(0, 0, 0);

	collisionPoints.push_back(this->calculateCollisionPoints());
	collisionPoints.push_back(other->calculateCollisionPoints());

	if (collisionPoints[0].empty() || collisionPoints[1].empty()) return false;

	std::vector<glm::vec3> axes;

	for (glm::vec3 normal : this->calculateCollisionNormals())
	{
		axes.push_back(normal);
	}

	for (glm::vec3 normal : other->calculateCollisionNormals())
	{
		axes.push_back(normal);
	}

	for (glm::vec3 axis : axes)
	{
		float aMaxProj = -std::numeric_limits<float>::infinity();
		float aMinProj = std::numeric_limits<float>::infinity();
		float bMaxProj = -std::numeric_limits<float>::infinity();
		float bMinProj = std::numeric_limits<float>::infinity();

		for (const glm::vec3& vertex : collisionPoints[0])
		{
			float projection = glm::dot(axis, vertex);
			if (projection < aMinProj) aMinProj = projection;
			if (projection > aMaxProj) aMaxProj = projection;
		}

		for (const glm::vec3& vertex : collisionPoints[1])
		{
			float projection = glm::dot(axis, vertex);
			if (projection < bMinProj) bMinProj = projection;
			if (projection > bMaxProj) bMaxProj = projection;
		}

		if (aMaxProj < bMinProj || aMinProj > bMaxProj)
		{
			return false;
		}

		float overlap = std::abs(projectedOverlap(aMinProj, aMaxProj, bMinProj, bMaxProj));

		if (overlap < minOverlap)
		{
			minOverlap = overlap;
			minOverlapAxis = axis;

			glm::vec3 direction = this->center - other->center;

			if (glm::dot(direction, minOverlapAxis) < 0)
			{
				minOverlapAxis = -minOverlapAxis;
			}
		}
	}

	CollidedObject collidedObject;
	glm::vec3 minimumTranslationVector = minOverlapAxis * minOverlap;

	if (glm::length(movement) > 0)
	{
		collisionResult->movementBeforeCollision = movement;
	}

	if (glm::length(minOverlapAxis) > 0 && glm::normalize(minOverlapAxis).y >= 0.9f)
	{
		movement.y = 0;
		onOtherObject = true;
		collidedObject.onTop = true;
	}
	else if (glm::length(minOverlapAxis) > 0 && glm::normalize(minOverlapAxis).y >= 0 &&
		other->getCollisionBoxType() == CollisionBoxType::prism)
	{
		minimumTranslationVector = minimumTranslationVector * glm::vec3(0, 1, 0);
	}

	collidedObject.movementBeforeCollision = other->movement;

	if (this->getType() & ObjectType::Object_Bullet)
	{
		this->position -= movement * glm::vec3(0.7f, 0.7f, 0.7f);
		this->movement = glm::vec3(0, 0, 0);
	}
	else if (!(this->getType() & ObjectType::Object_Player))
	{
		this->position += minimumTranslationVector;
	}

	collidedObject.object = other;
	collidedObject.MinimumTranslationVector = minimumTranslationVector;

	collisionResult->collided = true;
	collisionResult->collidedObjectList.push_back(std::move(collidedObject));

	return true;
}

void ObjectCore::reactToCollision(CollisionResult collisionResult, float32 deltaMs)
{
	float collisionSpeed = glm::length(collisionResult.movementBeforeCollision) * deltaMs / 1000.0f;

	if (collisionSpeed > 5)
	{
		addToHealth(-2 * collisionSpeed);
	}
}

void ObjectCore::calculationBeforeFrame()
{
}

void ObjectCore::calculationAfterFrame()
{
}

void ObjectCore::setDimensions(glm::vec3 newDimensions)
{
	dimensions = newDimensions;

	// Conservative AABB so rotated objects keep enough collision volume.
	boundingboxdimensions = glm::vec3(dimensions.x + dimensions.z,
		dimensions.y * 2,
		dimensions.x + dimensions.z);

	center = position;
	center.y += dimensions.y / 2;

	updateCollisionGeometry();
}

glm::vec3 ObjectCore::calculateDimensions()
{
	setDimensions(dimensions);
	return dimensions;
}

std::vector<glm::vec2> ObjectCore::calculateRectPoints()
{
	std::vector<glm::vec2> calculatedRectPoints;
	glm::vec2 newPoint;
	glm::vec2 newRotatedPoint;

	float cosW = cos(-rotation.y * 3.14f / 180.0f);
	float sinW = sin(-rotation.y * 3.14f / 180.0f);
	glm::mat2 rotationMatrix = glm::mat2(cosW, sinW, -sinW, cosW);

	newPoint = glm::vec2(dimensions.x / 2, dimensions.z / 2);
	newRotatedPoint = rotationMatrix * newPoint;
	newRotatedPoint.x += position.x;
	newRotatedPoint.y += position.z;
	calculatedRectPoints.push_back(newRotatedPoint);

	newPoint = glm::vec2(-dimensions.x / 2, dimensions.z / 2);
	newRotatedPoint = rotationMatrix * newPoint;
	newRotatedPoint.x += position.x;
	newRotatedPoint.y += position.z;
	calculatedRectPoints.push_back(newRotatedPoint);

	newPoint = glm::vec2(-dimensions.x / 2, -dimensions.z / 2);
	newRotatedPoint = rotationMatrix * newPoint;
	newRotatedPoint.x += position.x;
	newRotatedPoint.y += position.z;
	calculatedRectPoints.push_back(newRotatedPoint);

	newPoint = glm::vec2(dimensions.x / 2, -dimensions.z / 2);
	newRotatedPoint = rotationMatrix * newPoint;
	newRotatedPoint.x += position.x;
	newRotatedPoint.y += position.z;
	calculatedRectPoints.push_back(newRotatedPoint);

	return calculatedRectPoints;
}

std::vector<glm::vec3> ObjectCore::calculateCollisionPoints()
{
	glm::vec3 newPoint;
	glm::vec3 newRotatedPoint;
	cubePoints.clear();

	if (collisionBoxType == CollisionBoxType::cube || collisionBoxType == CollisionBoxType::none)
	{
		float cosW = cos(-rotation.y * 3.14f / 180.0f);
		float sinW = sin(-rotation.y * 3.14f / 180.0f);
		glm::mat3 rotationMatrix = glm::mat3(cosW, 0, sinW, 0, 1, 0, -sinW, 0, cosW);

		newPoint = glm::vec3(-dimensions.x / 2, 0, dimensions.z / 2);
		newRotatedPoint = rotationMatrix * newPoint;
		newRotatedPoint += position;
		cubePoints.push_back(newRotatedPoint);

		newPoint = glm::vec3(-dimensions.x / 2, 0, -dimensions.z / 2);
		newRotatedPoint = rotationMatrix * newPoint;
		newRotatedPoint += position;
		cubePoints.push_back(newRotatedPoint);

		newPoint = glm::vec3(dimensions.x / 2, 0, -dimensions.z / 2);
		newRotatedPoint = rotationMatrix * newPoint;
		newRotatedPoint += position;
		cubePoints.push_back(newRotatedPoint);

		newPoint = glm::vec3(dimensions.x / 2, 0, dimensions.z / 2);
		newRotatedPoint = rotationMatrix * newPoint;
		newRotatedPoint += position;
		cubePoints.push_back(newRotatedPoint);

		newPoint = glm::vec3(dimensions.x / 2, dimensions.y, dimensions.z / 2);
		newRotatedPoint = rotationMatrix * newPoint;
		newRotatedPoint += position;
		cubePoints.push_back(newRotatedPoint);

		newPoint = glm::vec3(dimensions.x / 2, dimensions.y, -dimensions.z / 2);
		newRotatedPoint = rotationMatrix * newPoint;
		newRotatedPoint += position;
		cubePoints.push_back(newRotatedPoint);

		newPoint = glm::vec3(-dimensions.x / 2, dimensions.y, -dimensions.z / 2);
		newRotatedPoint = rotationMatrix * newPoint;
		newRotatedPoint += position;
		cubePoints.push_back(newRotatedPoint);

		newPoint = glm::vec3(-dimensions.x / 2, dimensions.y, dimensions.z / 2);
		newRotatedPoint = rotationMatrix * newPoint;
		newRotatedPoint += position;
		cubePoints.push_back(newRotatedPoint);
	}
	else if (collisionBoxType == CollisionBoxType::prism)
	{
		float cosW = cos((-rotation.y - 90.0f) * 3.14f / 180.0f);
		float sinW = sin((-rotation.y - 90.0f) * 3.14f / 180.0f);
		glm::mat3 rotationMatrix = glm::mat3(cosW, 0, sinW, 0, 1, 0, -sinW, 0, cosW);

		newPoint = glm::vec3(-dimensions.x / 2, 0, dimensions.z / 2);
		newRotatedPoint = rotationMatrix * newPoint;
		newRotatedPoint += position;
		cubePoints.push_back(newRotatedPoint);

		newPoint = glm::vec3(-dimensions.x / 2, 0, -dimensions.z / 2);
		newRotatedPoint = rotationMatrix * newPoint;
		newRotatedPoint += position;
		cubePoints.push_back(newRotatedPoint);

		newPoint = glm::vec3(dimensions.x / 2, 0, -dimensions.z / 2);
		newRotatedPoint = rotationMatrix * newPoint;
		newRotatedPoint += position;
		cubePoints.push_back(newRotatedPoint);

		newPoint = glm::vec3(dimensions.x / 2, 0, dimensions.z / 2);
		newRotatedPoint = rotationMatrix * newPoint;
		newRotatedPoint += position;
		cubePoints.push_back(newRotatedPoint);

		newPoint = glm::vec3(dimensions.x / 2, dimensions.y, dimensions.z / 2);
		newRotatedPoint = rotationMatrix * newPoint;
		newRotatedPoint += position;
		cubePoints.push_back(newRotatedPoint);

		newPoint = glm::vec3(-dimensions.x / 2, dimensions.y, dimensions.z / 2);
		newRotatedPoint = rotationMatrix * newPoint;
		newRotatedPoint += position;
		cubePoints.push_back(newRotatedPoint);
	}

	return cubePoints;
}

std::vector<glm::vec3> ObjectCore::calculateCollisionNormals()
{
	std::vector<glm::vec3> normals;
	glm::vec3 normal;

	if (collisionBoxType == CollisionBoxType::cube)
	{
		if (cubePoints.size() >= 8)
		{
			normal = glm::normalize(cubePoints[1] - cubePoints[0]);
			normals.push_back(normal);

			normal = glm::normalize(cubePoints[2] - cubePoints[1]);
			normals.push_back(normal);

			normal = glm::normalize(cubePoints[7] - cubePoints[0]);
			normals.push_back(normal);
		}
	}
	else if (collisionBoxType == CollisionBoxType::prism)
	{
		if (cubePoints.size() >= 6)
		{
			normal = glm::normalize(cubePoints[1] - cubePoints[0]);
			normals.push_back(normal);

			normal = glm::normalize(cubePoints[2] - cubePoints[1]);
			normals.push_back(normal);

			normal = glm::normalize(cubePoints[5] - cubePoints[0]);
			normals.push_back(normal);

			normal = glm::normalize(glm::cross(
				glm::normalize(cubePoints[5] - cubePoints[4]),
				glm::normalize(cubePoints[4] - cubePoints[2])
			));
			normals.push_back(normal);
		}
	}

	return normals;
}

std::vector<glm::vec2> ObjectCore::getRectPoints() const
{
	return rectPoints;
}

std::vector<glm::vec3> ObjectCore::getCubePoints() const
{
	return cubePoints;
}

std::vector<glm::vec3> ObjectCore::getCubeNormals() const
{
	return cubeNormals;
}

bool ObjectCore::checkBoundaries(std::shared_ptr<ObjectCore> map, float32 deltaMs)
{
	if (!map) return false;

	bool outOfBounds = false;
	glm::vec3 newPosition = position + movement * deltaMs * 0.001f;

	// X axis
	if (newPosition.x - dimensions.x / 2 < map->getPosition().x - map->getDimensions().x / 2)
	{
		outOfBounds = true;
		movement.x = 0;
	}

	if (newPosition.x + dimensions.x / 2 > map->getPosition().x + map->getDimensions().x / 2)
	{
		outOfBounds = true;
		movement.x = 0;
	}

	// Y axis
	if (newPosition.y / 2 < map->getPosition().y - 1)
	{
		outOfBounds = true;
		position.y = 0;
		movement.y = 0;
	}

	if (newPosition.y + dimensions.y / 2 > map->getPosition().y + 200 / 2)
	{
		outOfBounds = true;
		movement.y = 0;
	}

	// Z axis
	if (newPosition.z - dimensions.z / 2 < map->getPosition().z - map->getDimensions().z / 2)
	{
		outOfBounds = true;
		movement.z = 0;
	}

	if (newPosition.z + dimensions.z / 2 > map->getPosition().z + map->getDimensions().z / 2)
	{
		outOfBounds = true;
		movement.z = 0;
	}

#ifdef DEBUG_OUTOFBOUNDS
	if (outOfBounds)
	{
		Logger::log("Object out of Bounds: " + printObject());
	}
#endif

	return outOfBounds;
}

void ObjectCore::fall(float32 deltaMs, glm::vec3 gravity)
{
	if (!this->gravity) return;

	if (position.y > 0 && !onOtherObject)
	{
		movement += gravity * deltaMs * 0.001f;
	}
}

void ObjectCore::move(float32 deltaMs, std::shared_ptr<ObjectCore> map)
{
	checkBoundaries(map, deltaMs);

	position += movement * deltaMs * 0.001f;

	center = position;
	center.y += dimensions.y / 2;

	if (movement != glm::vec3(0, 0, 0))
	{
		onOtherObject = false;
	}
}

void ObjectCore::setPosition(glm::vec3 newPosition)
{
	position = newPosition;
	updateCollisionGeometry();
}

glm::vec3 ObjectCore::getPosition() const
{
	return position;
}

void ObjectCore::setRotation(glm::vec3 newRotation)
{
	rotation = newRotation;
	updateCollisionGeometry();
}

glm::vec3 ObjectCore::getRotation() const
{
	return rotation;
}

void ObjectCore::setScale(glm::vec3 newScale)
{
	scale = newScale;

	dimensions = dimensions * scale;
	setDimensions(dimensions);
}

glm::vec3 ObjectCore::getScale() const
{
	return scale;
}

void ObjectCore::setMovement(glm::vec3 newMovement)
{
	movement = newMovement;
}

glm::vec3 ObjectCore::getMovement() const
{
	return movement;
}

glm::vec3 ObjectCore::getDimensions() const
{
	return dimensions;
}

glm::vec3 ObjectCore::getBoundingBoxDimensions() const
{
	return boundingboxdimensions;
}

void ObjectCore::setType(ObjectType newObjectType)
{
	type = newObjectType;
}

ObjectType ObjectCore::getType() const
{
	return type;
}

ObjectType ObjectCore::convertStringToType(std::string objectTypeAsString)
{
	for (const auto& pair : objectTypeTable)
	{
		if (pair.first == objectTypeAsString)
		{
			return pair.second;
		}
	}

	return ObjectType::Object_Entity;
}

void ObjectCore::setName(std::string newName)
{
	name = newName;
}

std::string ObjectCore::getName() const
{
	return name;
}

void ObjectCore::setNumber(int32 newNumber)
{
	number = newNumber;
}

int32 ObjectCore::getNumber() const
{
	return number;
}

void ObjectCore::registerHit()
{
	addToHealth(-20);
	lastHitTimestamp = std::chrono::system_clock::now();
}

void ObjectCore::setHealth(float32 newHealth)
{
	health = newHealth;
}

float32 ObjectCore::getHealth() const
{
	return health;
}

void ObjectCore::addToHealth(float32 addHealth)
{
	health += addHealth;

	if (addHealth > 0)
	{
		Logger::log(printObject() + " got healed by " + std::to_string((int)addHealth) + ". New Health: " + std::to_string((int)health));
	}

	if (addHealth < 0)
	{
		Logger::log(printObject() + " got " + std::to_string((int)addHealth) + " damage. New Health: " + std::to_string((int)health));
	}

	if (health <= 0)
	{
		Logger::log(printObject() + " got destroyed");
	}
}

std::string ObjectCore::printObject() const
{
	return name + " with Number " + std::to_string(number);
}

std::string ObjectCore::printPosition() const
{
	return "Position: " + std::to_string(position.x) + " " + std::to_string(position.y) + " " + std::to_string(position.z);
}

void ObjectCore::setCollisionBoxType(CollisionBoxType newCollisionBoxType)
{
	collisionBoxType = newCollisionBoxType;
	updateCollisionGeometry();
}

CollisionBoxType ObjectCore::getCollisionBoxType() const
{
	return collisionBoxType;
}

CollisionBoxType ObjectCore::convertStringToCollisionBoxType(std::string collisionBoxTypeAsString)
{
	for (const auto& pair : CollisionBoxTypeTable)
	{
		if (pair.first == collisionBoxTypeAsString)
		{
			return pair.second;
		}
	}

	return CollisionBoxType::none;
}

void ObjectCore::setGravity(bool enable)
{
	gravity = enable;
}

bool ObjectCore::getGravity() const
{
	return gravity;
}

bool ObjectCore::isGettingDamaged() const
{
	std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
	std::chrono::duration<double, std::milli> notGettingHitDuration = now - lastHitTimestamp;

	return notGettingHitDuration.count() < 1000;
}

void ObjectCore::updateCollisionGeometry()
{
	rectPoints = calculateRectPoints();
	cubePoints = calculateCollisionPoints();
	cubeNormals = calculateCollisionNormals();
}