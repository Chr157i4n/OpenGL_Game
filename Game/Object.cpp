#include "Object.h"

#include "UI.h"
#include "Logger.h"
#include "ResourceManager.h"
#include "Game.h"
#include "Logger.h"
#include "Raypicker.h"

float projectedOverlap(float minA, float maxA, float minB, float maxB) {
	return (std::max)(0.0f, (std::min)(maxA, maxB) - (std::max)(minA, minB));
}




Object::Object(Shader* shader, std::string modelFileName)
{
	setType(ObjectType::Object_Entity);

	position = glm::vec3(0, 0, 0);
	rotation = glm::vec3(0, 0, 0);
	movement = glm::vec3(0, 0, 0);
	dimensions = glm::vec3(2, 2, 2);

	this->shader = shader;
	this->model = ResourceManager::getModelByName(modelFileName);


	dimensions = calculateDimensions();
	rectPoints = calculateRectPoints();
	cubePoints = calculateCollisionPoints();
	cubeNormals = calculateCollisionNormals();

	center = position;
	center.y += dimensions.y / 2;

	this->name = "Object";

	this->setGravity(true);
}

Shader* Object::getShader()
{
	return shader;
}

void Object::bindShader()
{
	shader->bind();
}

void Object::unbindShader()
{
	shader->unbind();
}


bool  Object::intersectWithRay(Ray ray)
{
	float minX = this->getPosition().x - this->getDimensions().x/2;
	float maxX = this->getPosition().x + this->getDimensions().x/2;

	float minY = this->getPosition().y;
	float maxY = this->getPosition().y + this->getDimensions().y;

	float minZ = this->getPosition().z - this->getDimensions().z/2;
	float maxZ = this->getPosition().z + this->getDimensions().z/2;

	
	float tmin = (minX - ray.origin.x) / ray.direction.x;
	float tmax = (maxX - ray.origin.x) / ray.direction.x;

	if (tmin > tmax) std::swap(tmin, tmax);

	float tymin = (minY - ray.origin.y) / ray.direction.y;
	float tymax = (maxY - ray.origin.y) / ray.direction.y;

	if (tymin > tymax) std::swap(tymin, tymax);

	if ((tmin > tymax) || (tymin > tmax))
		return false;

	if (tymin > tmin)
		tmin = tymin;

	if (tymax < tmax)
		tmax = tymax;

	float tzmin = (minZ - ray.origin.z) / ray.direction.z;
	float tzmax = (maxZ - ray.origin.z) / ray.direction.z;

	if (tzmin > tzmax) std::swap(tzmin, tzmax);

	if ((tmin > tzmax) || (tzmin > tmax))
		return false;

	if (tzmin > tmin)
		tmin = tzmin;

	if (tzmax < tmax)
		tmax = tzmax;

	//Ray is intersecting with plane
	//test for right direction

	if (glm::dot(ray.direction, this->getPosition()- ray.origin) < 0)
	{
		//Logger::log("facing wrong direction");
		return false;
	}
	else
	{
		//Logger::log("facing right direction");
		return true;
	}
}

float Object::getDistance(std::shared_ptr<Object> object)
{
	return std::abs(glm::length(object->getCenter() - this->getCenter()));
}

float Object::getDistance(glm::vec3 location)
{
	return std::abs(glm::length(location - this->getCenter()));
}

void Object::markObject()
{
	lastMarkTimestamp = std::chrono::system_clock::now();
}

void Object::markObject(float duration)
{
	this->markDuration = duration;
	lastMarkTimestamp = std::chrono::system_clock::now();
}

bool Object::isMarked()
{
	std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
	std::chrono::duration<double, std::milli> notGettigHitDuration = now - lastMarkTimestamp;
	if (notGettigHitDuration.count() > markDuration)
	{
		return false;
	}
	else
	{
		return true;
	}
}

void Object::interact_click()
{
	if (!this->getEnabled()) return;
}

void Object::interact_hold()
{
	if (!this->getEnabled()) return;
}

CollisionResult Object::checkCollision()
{
	
	
	CollisionResult collisionResult;
	collisionResult.collided = false;

	if (this->movement == glm::vec3(0, 0, 0)) return collisionResult;

	for (std::shared_ptr<Object> object : Game::objects)
	{
		if (!object->getEnabled()) continue;
		if (object->getNumber() == this->getNumber()) continue;		//dont check collision with yourself
		if (object->getType() & ObjectType::Object_Bullet) continue;	//bullets move faster than everything else, so only bullets need to check collision wiht other objects
		if (object->getCollisionBoxType() == CollisionBoxType::none) continue;	//if the object has "none" collision box, no collision should be checked

		if (checkCollision_AABB(object))
		{
			checkCollision_SAT(object, &collisionResult);
		}
	}

	return collisionResult;
}

bool Object::checkCollision_AABB(std::shared_ptr < Object> object)
{
	//calculate the min/max values for all three axis for both objects
	float object1MaxX = this->position.x + boundingboxdimensions.x / 2;
	float object1MinX = this->position.x - boundingboxdimensions.x / 2;

	float object1MaxY = this->position.y + this->dimensions.y / 2 + boundingboxdimensions.y / 2;
	float object1MinY = this->position.y + this->dimensions.y / 2 - boundingboxdimensions.y / 2;

	float object1MaxZ = this->position.z + boundingboxdimensions.z / 2;
	float object1MinZ = this->position.z - boundingboxdimensions.z / 2;

	glm::vec3 object2Position = object->getPosition();
	glm::vec3 object2BoundingBoxDimension = object->getBoundingBoxDimensions();
	glm::vec3 object2Rotation = object->getRotation();

	float object2MinX = object2Position.x - (object2BoundingBoxDimension.x / 2);
	float object2MaxX = object2Position.x + (object2BoundingBoxDimension.x / 2);

	float object2MinY = object2Position.y + this->dimensions.y / 2 - (object2BoundingBoxDimension.y / 2);
	float object2MaxY = object2Position.y + this->dimensions.y / 2 + (object2BoundingBoxDimension.y / 2);

	float object2MinZ = object2Position.z - (object2BoundingBoxDimension.z / 2);
	float object2MaxZ = object2Position.z + (object2BoundingBoxDimension.z / 2);

	//Compare these values to check whether the BoundingBoxes collide
	if (object1MaxX < object2MinX)
		return false;

	if (object1MinX > object2MaxX)
		return false;

	if (object1MaxY < object2MinY)
		return false;

	if (object1MinY > object2MaxY)
		return false;

	if (object1MaxZ < object2MinZ)
		return false;

	if (object1MinZ > object2MaxZ)
		return false;

#ifdef DEBUG_COLLISION
	Logger::log("AABB collision:"+ this->printObject() +" - "+ object->printObject());
#endif
	return true;
}

bool Object::checkCollision_SAT(std::shared_ptr < Object> object, CollisionResult* collisionResult)
{
	std::vector<std::vector<glm::vec3>> collisionPoints;
	float minOverlap = std::numeric_limits<float>::infinity();
	glm::vec3 minOverlapAxis = glm::vec3(0, 0, 0);
	//Logger::log("SAT calculation");

	collisionPoints.push_back(this->getCollisionPoints());
	collisionPoints.push_back(object->getCollisionPoints());


	std::vector<glm::vec3> axes;

	for (glm::vec3 normal : this->calculateCollisionNormals())
	{
		axes.push_back(normal);
	}
	for (glm::vec3 normal : object->calculateCollisionNormals())
	{
		axes.push_back(normal);
	}


	for (glm::vec3 axis : axes)
	{
		// loop over all vertices of both polygons and project them
					// onto the axis. We are only interested in max/min projections
		float aMaxProj = -std::numeric_limits<float>::infinity();
		float aMinProj = std::numeric_limits<float>::infinity();
		float bMaxProj = -std::numeric_limits<float>::infinity();
		float bMinProj = std::numeric_limits<float>::infinity();
		for (const auto& v : collisionPoints[0]) {
			auto proj = glm::dot(axis, v);
			if (proj < aMinProj) aMinProj = proj;
			if (proj > aMaxProj) aMaxProj = proj;
		}

		for (const auto& v : collisionPoints[1]) {
			auto proj = glm::dot(axis, v);
			if (proj < bMinProj) bMinProj = proj;
			if (proj > bMaxProj) bMaxProj = proj;
		}

		// now check if the intervals the both polygons projected on the
		// axis overlap. If they don't, we have found an axis of separation and
		// the given polygons cannot overlap
		if (aMaxProj < bMinProj || aMinProj > bMaxProj)
		{
			//Logger::log("No SAT Collision");
			return false;
		}
		//at this line the objects are colliding for this projected axis
		float overlap = projectedOverlap(aMinProj, aMaxProj, bMinProj, bMaxProj);

		//Logger::log("overlap: "+ std::to_string(overlap));
		overlap = std::abs(overlap);
		if (overlap < minOverlap) {
			minOverlap = overlap;
			minOverlapAxis = axis;

			glm::vec3 d = this->center - object->center;
			if (glm::dot(d, minOverlapAxis) < 0)
				minOverlapAxis = -minOverlapAxis;
		}


	}


	// at this point, we have checked all axis but found no separating axis
	// which means that the polygons must intersect.
#ifdef DEBUG_COLLISION
	Logger::log("SAT collision: " + this->printObject() + this->printPosition() + " - " + object->printObject() + object->printPosition());
	Logger::log("minOverlapAxis:" + std::to_string(minOverlapAxis.x) + "," + std::to_string(minOverlapAxis.y) + "," + std::to_string(minOverlapAxis.z));
	Logger::log("minOverlap:" + std::to_string(minOverlap));
#endif

	CollidedObject* collidedObject = new CollidedObject();
	glm::vec3 MinimumTranslationVector = minOverlapAxis * minOverlap;

	if(glm::length(movement)>0)
		collisionResult->movementBeforeCollision = movement;

	if (glm::normalize(minOverlapAxis).y >= 0.9)
	{
#ifdef DEBUG_COLLISION
		Logger::log("standing on Object");
#endif
		movement.y = 0;
		onOtherObject = true;
		collidedObject->onTop = true;
	}
	else if (glm::normalize(minOverlapAxis).y >= 0 && object->getCollisionBoxType() == CollisionBoxType::prism)
	{
		MinimumTranslationVector = MinimumTranslationVector * glm::vec3(0, 1, 0);
	}


	collidedObject->movementBeforeCollision = object->movement;
	

	if (this->getType() & ObjectType::Object_Bullet)
	{
		this->position -= movement * glm::vec3(0.7, 0.7, 0.7);
		this->movement = glm::vec3(0, 0, 0);
	}
	else {
		this->position += MinimumTranslationVector;
	}

	collidedObject->object = object;
	collidedObject->MinimumTranslationVector = MinimumTranslationVector;

	collisionResult->collided = true;
	collisionResult->collidedObjectList.push_back(collidedObject);


	return true;

}

void Object::reactToCollision(CollisionResult collisionResult)
{
	float collisionSpeed = glm::length(collisionResult.movementBeforeCollision)* Game::getDelta()/1000.0f;
	if (collisionSpeed > 5)
	{
		addToHealth(-2 * collisionSpeed);
	}
}

void Object::calculationBeforeFrame()
{
	this->calculateCollisionPoints();
}

void Object::calculationAfterFrame()
{

}

glm::vec3 Object::calculateDimensions()
{
	dimensions = model->getDimension();
	boundingboxdimensions = model->getBoundingBoxDimension();
	//Logger::log(printObject() + " Dimensions: x: " + std::to_string(dimensions.x) + " y: " + std::to_string(dimensions.y) + " z: " + std::to_string(dimensions.z));
	return dimensions;
}

std::vector<glm::vec2> Object::calculateRectPoints()
{
	std::vector<glm::vec2> rectPoints;
	glm::vec2 newPoint;
	glm::vec2 newRotatedPoint;

	float cosW = cos(-getRotation().y * 3.14 / 180);
	float sinW = sin(-getRotation().y * 3.14 / 180);
	glm::mat2 rotationmatrix = glm::mat2(cosW, sinW, -sinW, cosW);
	//point

	newPoint = glm::vec2(getDimensions().x / 2, getDimensions().z / 2);


	newRotatedPoint = rotationmatrix * newPoint;

	newRotatedPoint.x += getPosition().x;
	newRotatedPoint.y += getPosition().z;

	rectPoints.push_back(newRotatedPoint);

	//point
	newPoint = glm::vec2(-getDimensions().x / 2, getDimensions().z / 2);

	newRotatedPoint = rotationmatrix * newPoint;

	newRotatedPoint.x += getPosition().x;
	newRotatedPoint.y += getPosition().z;

	rectPoints.push_back(newRotatedPoint);

	//point
	newPoint = glm::vec2(-getDimensions().x / 2, -getDimensions().z / 2);


	newRotatedPoint = rotationmatrix * newPoint;

	newRotatedPoint.x += getPosition().x;
	newRotatedPoint.y += getPosition().z;

	rectPoints.push_back(newRotatedPoint);

	//point
	newPoint = glm::vec2(getDimensions().x / 2, -getDimensions().z / 2);

	newRotatedPoint = rotationmatrix * newPoint;

	newRotatedPoint.x += getPosition().x;
	newRotatedPoint.y += getPosition().z;

	rectPoints.push_back(newRotatedPoint);

	return rectPoints;
}

std::vector<glm::vec3> Object::calculateCollisionPoints()
{
	glm::vec3 newPoint;
	glm::vec3 newRotatedPoint;
	cubePoints.clear();

	if (this->getCollisionBoxType() == CollisionBoxType::cube || this->getCollisionBoxType() == CollisionBoxType::none)
	{
		float cosW = cos(-rotation.y * 3.14 / 180);
		float sinW = sin(-rotation.y * 3.14 / 180);
		//glm::mat3 rotationmatrix1 = glm::mat3(1, 0, 0, 0, cosW, -sinW, 0, sinW, cosW); // around the x-axis
		glm::mat3 rotationmatrix1 = glm::mat3(cosW, 0, sinW, 0, 1, 0, -sinW, 0, cosW); // around the y-axis
		//glm::mat3 rotationmatrix1 = glm::mat3(cosW, -sinW, 0, sinW, cosW, 0, 0, 0, 1); // around the z-axis

		//bottom
		//point
		newPoint = glm::vec3(-getDimensions().x / 2, 0, getDimensions().z / 2);
		newRotatedPoint = rotationmatrix1 * newPoint;
		newRotatedPoint += glm::vec3(getPosition().x, getPosition().y, getPosition().z);
		cubePoints.push_back(newRotatedPoint);

		//point
		newPoint = glm::vec3(-getDimensions().x / 2, 0, -getDimensions().z / 2);
		newRotatedPoint = rotationmatrix1 * newPoint;
		newRotatedPoint += glm::vec3(getPosition().x, getPosition().y, getPosition().z);
		cubePoints.push_back(newRotatedPoint);

		//point
		newPoint = glm::vec3(getDimensions().x / 2, 0, -getDimensions().z / 2);
		newRotatedPoint = rotationmatrix1 * newPoint;
		newRotatedPoint += glm::vec3(getPosition().x, getPosition().y, getPosition().z);
		cubePoints.push_back(newRotatedPoint);

		//point
		newPoint = glm::vec3(getDimensions().x / 2, 0, getDimensions().z / 2);
		newRotatedPoint = rotationmatrix1 * newPoint;
		newRotatedPoint += glm::vec3(getPosition().x, getPosition().y, getPosition().z);
		cubePoints.push_back(newRotatedPoint);

		//top
		//point
		newPoint = glm::vec3(getDimensions().x / 2, getDimensions().y, getDimensions().z / 2);
		newRotatedPoint = rotationmatrix1 * newPoint;
		newRotatedPoint += glm::vec3(getPosition().x, getPosition().y, getPosition().z);
		cubePoints.push_back(newRotatedPoint);

		//point
		newPoint = glm::vec3(getDimensions().x / 2, getDimensions().y, -getDimensions().z / 2);
		newRotatedPoint = rotationmatrix1 * newPoint;
		newRotatedPoint += glm::vec3(getPosition().x, getPosition().y, getPosition().z);
		cubePoints.push_back(newRotatedPoint);

		//point
		newPoint = glm::vec3(-getDimensions().x / 2, getDimensions().y, -getDimensions().z / 2);
		newRotatedPoint = rotationmatrix1 * newPoint;
		newRotatedPoint += glm::vec3(getPosition().x, getPosition().y, getPosition().z);
		cubePoints.push_back(newRotatedPoint);

		//point
		newPoint = glm::vec3(-getDimensions().x / 2, getDimensions().y, getDimensions().z / 2);
		newRotatedPoint = rotationmatrix1 * newPoint;
		newRotatedPoint += glm::vec3(getPosition().x, getPosition().y, getPosition().z);
		cubePoints.push_back(newRotatedPoint);
	}
	else if (this->getCollisionBoxType() == CollisionBoxType::prism)
	{
		float cosW = cos((-rotation.y - 90) * 3.14 / 180);
		float sinW = sin((-rotation.y - 90) * 3.14 / 180);
		//glm::mat3 rotationmatrix1 = glm::mat3(1, 0, 0, 0, cosW, -sinW, 0, sinW, cosW); // around the x-axis
		glm::mat3 rotationmatrix1 = glm::mat3(cosW, 0, sinW, 0, 1, 0, -sinW, 0, cosW); // around the y-axis
		//glm::mat3 rotationmatrix1 = glm::mat3(cosW, -sinW, 0, sinW, cosW, 0, 0, 0, 1); // around the z-axis

		//bottom
		//point
		newPoint = glm::vec3(-getDimensions().x / 2, 0, getDimensions().z / 2);
		newRotatedPoint = rotationmatrix1 * newPoint;
		newRotatedPoint += glm::vec3(getPosition().x, getPosition().y, getPosition().z);
		cubePoints.push_back(newRotatedPoint);

		//point
		newPoint = glm::vec3(-getDimensions().x / 2, 0, -getDimensions().z / 2);
		newRotatedPoint = rotationmatrix1 * newPoint;
		newRotatedPoint += glm::vec3(getPosition().x, getPosition().y, getPosition().z);
		cubePoints.push_back(newRotatedPoint);

		//point
		newPoint = glm::vec3(getDimensions().x / 2, 0, -getDimensions().z / 2);
		newRotatedPoint = rotationmatrix1 * newPoint;
		newRotatedPoint += glm::vec3(getPosition().x, getPosition().y, getPosition().z);
		cubePoints.push_back(newRotatedPoint);

		//point
		newPoint = glm::vec3(getDimensions().x / 2, 0, getDimensions().z / 2);
		newRotatedPoint = rotationmatrix1 * newPoint;
		newRotatedPoint += glm::vec3(getPosition().x, getPosition().y, getPosition().z);
		cubePoints.push_back(newRotatedPoint);

		//top
		//point
		newPoint = glm::vec3(getDimensions().x / 2, getDimensions().y, getDimensions().z / 2);
		newRotatedPoint = rotationmatrix1 * newPoint;
		newRotatedPoint += glm::vec3(getPosition().x, getPosition().y, getPosition().z);
		cubePoints.push_back(newRotatedPoint);

		//point
		newPoint = glm::vec3(-getDimensions().x / 2, getDimensions().y, getDimensions().z / 2);
		newRotatedPoint = rotationmatrix1 * newPoint;
		newRotatedPoint += glm::vec3(getPosition().x, getPosition().y, getPosition().z);
		cubePoints.push_back(newRotatedPoint);
	}

	#pragma region orientedBoundingBox
	orientedBoundingBox.minX =  std::numeric_limits<float>::infinity();
	orientedBoundingBox.maxX = -std::numeric_limits<float>::infinity();
	orientedBoundingBox.minY =  std::numeric_limits<float>::infinity();
	orientedBoundingBox.maxY = -std::numeric_limits<float>::infinity();
	orientedBoundingBox.minZ =  std::numeric_limits<float>::infinity();
	orientedBoundingBox.maxZ = -std::numeric_limits<float>::infinity();
	for (glm::vec3 point : cubePoints)
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

	#pragma endregion

	return cubePoints;
}

std::vector<glm::vec3> Object::calculateCollisionNormals()
{
	std::vector<glm::vec3> normals;
	glm::vec3 normal;

	if (this->getCollisionBoxType() == CollisionBoxType::cube)
	{
		normal = (glm::normalize(cubePoints[1] - cubePoints[0]));
		normals.push_back(normal);

		normal = (glm::normalize(cubePoints[2] - cubePoints[1]));
		normals.push_back(normal);

		normal = (glm::normalize(cubePoints[7] - cubePoints[0]));
		normals.push_back(normal);
	}
	else if (this->getCollisionBoxType() == CollisionBoxType::prism)
	{
		normal = (glm::normalize(cubePoints[1] - cubePoints[0]));
		normals.push_back(normal);

		normal = (glm::normalize(cubePoints[2] - cubePoints[1]));
		normals.push_back(normal);

		normal = (glm::normalize(cubePoints[5] - cubePoints[0]));
		normals.push_back(normal);

		normal = (glm::normalize(glm::cross(glm::normalize(cubePoints[5] - cubePoints[4]), glm::normalize(cubePoints[4] - cubePoints[2]))));
		normals.push_back(normal);
	}

	return normals;
}

std::vector<glm::vec2> Object::getRectPoints()
{
	return rectPoints;
}

std::vector<glm::vec3> Object::getCubePoints()
{
	return cubePoints;
}

std::vector<glm::vec3> Object::getCubeNormals()
{
	return cubeNormals;
}

bool Object::checkBoundaries()
{
	std::shared_ptr<Object> map = Game::map[0];
	bool outOfBounds = false;
	glm::vec3 newPosition = position + movement * Game::getDelta() * 0.001f;;

	// X - Achse
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


	// Y - Achse
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


	// Z - Achse
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
#endif // DEBUG_OUTOFBOUNDS
	return outOfBounds;
}

void Object::fall()
{
	if (!this->getGravity()) return;

	if (position.y > 0 && !onOtherObject)
	{
#ifdef DEBUG_GRAVITY
		Logger::log("Object: " + printObject() + " is falling");
#endif
		movement += Map::getGravity() * Game::getDelta() * 0.001f;
	}


}

void Object::move()
{
	checkBoundaries();

	position += movement * Game::getDelta() * 0.001f;

	center = position;
	center.y += dimensions.y / 2;

	//if the objects moves onOtherObject should be calculated new
	if (movement != glm::vec3(0, 0, 0))
	{
		onOtherObject = false;
	}
}

void Object::setPosition(glm::vec3 newPosition)
{
	position = newPosition;

	center = position;
	center.y += dimensions.y / 2;
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

void Object::setScale(glm::vec3 newScale)
{
	scale = newScale;
}

glm::vec3 Object::getScale()
{
	return scale;
}

void Object::setMovement(glm::vec3 newMovement)
{
	movement = newMovement;
}

glm::vec3 Object::getMovement()
{
	return movement;
}

glm::vec3 Object::getDimensions()
{
	return dimensions;
}

glm::vec3 Object::getBoundingBoxDimensions()
{
	return boundingboxdimensions;
}

void Object::setType(ObjectType newObjectType)
{
	type = newObjectType;
}

ObjectType Object::getType()
{
	return type;
}

ObjectType Object::convertStringToType(std::string objectTypeAsString)
{
	auto it = objectTypeTable.find(objectTypeAsString);
	if (it != objectTypeTable.end()) {
		return it->second;
	}
}

void Object::setName(std::string newName)
{
	name = newName;
}

std::string Object::getName()
{
	return name;
}

void Object::setNumber(int32 newNumber)
{
	number = newNumber;
}

int32 Object::getNumber()
{
	return number;
}

void Object::render()
{
	glm::vec2 currentTextureFlowPosition = textureFlow * glm::vec2(Game::getTimeStamp()*0.001, Game::getTimeStamp()*0.001);
	int textureFlowUniformLocation = GLCALL(glGetUniformLocation(Renderer::getShader(ShaderType::basic)->getShaderId(), "u_textureflow"));
	GLCALL(glUniform2fv(textureFlowUniformLocation, 1, &currentTextureFlowPosition[0] ));

	this->model->render();
}

void Object::renderShadowMap()
{
	this->model->renderShadowMap();
}

void Object::renderEnvMap()
{
	this->model->renderEnvMap();
}

void Object::registerHit()
{
	addToHealth(-20);
	lastHitTimestamp = std::chrono::system_clock::now();
}

bool Object::isGettingDamaged()
{
	std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
	std::chrono::duration<double, std::milli> notGettigHitDuration = now - lastHitTimestamp;
	if (notGettigHitDuration.count() > 1000)
	{
		return false;
	}
	else
	{
		return true;
	}
}

void Object::setHealth(float32 newHealth)
{
	health = newHealth;
}

float32 Object::getHealth()
{
	return health;
}

void Object::addToHealth(float32 addHealth)
{
	health += addHealth;

	if (addHealth > 0)
		Logger::log(printObject() + " got healed by " + std::to_string((int)addHealth) + ". New Health: " + std::to_string((int)health));
	if (addHealth < 0)
		Logger::log(printObject() + " got " + std::to_string((int)addHealth) + " damage. New Health: " + std::to_string((int)health));

	if (health <= 0)
	{
		Logger::log(printObject() + " got destroyed");
	}
}

std::string Object::printObject()
{
	if (!this) return "NA";

	return "[" + std::to_string(getNumber()) + "|" + getName() + "|" + std::to_string(getType()) + "]";	
}

std::string Object::printPosition()
{
	return "[" + std::to_string(position.x) + "|" + std::to_string(position.y) + "|" + std::to_string(position.z) + "]";
}

void Object::setCollisionBoxType(CollisionBoxType collisionBoxType)
{
	this->collisionBoxType = collisionBoxType;
}

CollisionBoxType Object::getCollisionBoxType()
{
	return collisionBoxType;
}

CollisionBoxType Object::convertStringToCollisionBoxType(std::string collissionBoxTypeAsString)
{
	auto it = CollisionBoxTypeTable.find(collissionBoxTypeAsString);
	if (it != CollisionBoxTypeTable.end()) {
		return it->second;
	}
}

void Object::setGravity(bool enable)
{
	gravity = enable;
}

bool Object::getGravity()
{
	return gravity;
}

void Object::setModel(Model* newModel)
{
	model = newModel;
}

void Object::setModel(std::string newModelFileName)
{
	this->model = ResourceManager::getModelByName(newModelFileName);
}

Model* Object::getModel()
{
	return model;
}