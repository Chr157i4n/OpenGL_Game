#include "Object.h"
#include "UI.h"
#include <algorithm>
#include "Logger.h"
#include "libs/glm/gtx/rotate_vector.hpp"


float projectedOverlap(float minA, float maxA, float minB, float maxB) {
	if (minA < minB) {
		return minB - maxA;
	}
	else {
		return minA - maxB;
	}
}




Object::Object(Shader* shader, std::string modelFileName)
{
	setType(ObjectType::Object_Entity);

	position = glm::vec3(0, 0, 0);
	rotation = glm::vec3(0, 0, 0);
	movement = glm::vec3(0, 0, 0);
	dimensions = glm::vec3(2, 2, 2);

	this->shader = shader;

	model = new Model;
	
	loadModel(modelFileName);
	dimensions = calculateDimensions();
	rectPoints = calculateRectPoints();
	cubePoints = calculateCollisionPoints();
	cubeNormals = calculateCollisionNormals();

	center = position;
	center.y += dimensions.y / 2;

	this->name = "Object";
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


CollisionResult Object::checkCollision(std::vector<Object*> objects)
{
	CollisionResult collisionResult;
	collisionResult.collided = false;
	
	if (this->movement == glm::vec3(0, 0, 0)) return collisionResult;

	for (Object* object : objects)
	{
		if (object->getNumber() == this->getNumber()) continue;
		//if (object->getType() == ObjectType::Object_Environment) continue;
		if (object->getType() == ObjectType::Object_Bullet) continue;
		//if (object->getType() == ObjectType::Object_Player) continue;

		if (checkCollision_AABB(object))
		{
			checkCollision_SAT(object, &collisionResult);
		}
	}

	return collisionResult;

}

bool Object::checkCollision_AABB(Object* object)
{
	//calculate the min/max values for all three axis for both objects
	float object1MaxX = this->position.x + boundingboxdimensions.x / 2;
	float object1MinX = this->position.x - boundingboxdimensions.x / 2;

	float object1MaxY = this->position.y + this->dimensions.y/2 + boundingboxdimensions.y / 2;
	float object1MinY = this->position.y + this->dimensions.y/2 - boundingboxdimensions.y / 2;

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

	//Logger::log("AABB collision between: [MFN: " + getName() + ",OT: " + std::to_string(getType()) + ",ON: " + std::to_string(getNumber()) + "]  and  [MFN: " + object->getName() + ",OT: " + std::to_string(object->getType()) + ",ON: " + std::to_string(object->getNumber()) + "]");
	return true;
}

bool Object::checkCollision_SAT(Object* object, CollisionResult* collisionResult)
{
	std::vector<std::vector<glm::vec3>> CollisionPoints;
	float minOverlap = std::numeric_limits<float>::infinity();
	glm::vec3 minOverlapAxis = glm::vec3(0,0,0);
	

		CollisionPoints.push_back(this->calculateCollisionPoints());
		CollisionPoints.push_back(object->calculateCollisionPoints());


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
		for (const auto& v : CollisionPoints[0]) {
			auto proj = glm::dot(axis, v);
			if (proj < aMinProj) aMinProj = proj;
			if (proj > aMaxProj) aMaxProj = proj;
		}

		for (const auto& v : CollisionPoints[1]) {
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

		overlap = std::abs(overlap);
		if (overlap < minOverlap) {
			minOverlap = overlap;
			minOverlapAxis = axis;

			glm::vec3 d = this->center - object->center;
			if (glm::dot(d,minOverlapAxis) <0)
				minOverlapAxis = -minOverlapAxis;
		}

		
	}


	// at this point, we have checked all axis but found no separating axis
	// which means that the polygons must intersect.
	Logger::log("SAT collision between: "+this->printObject()+" and  "+object->printObject());
	
	Logger::log("minOverlapAxis:" + std::to_string(minOverlapAxis.x)+","+ std::to_string(minOverlapAxis.y) + "," + std::to_string(minOverlapAxis.z));
	Logger::log("minOverlap:" + std::to_string(minOverlap));

	glm::vec3 MinimumTranslationVector = minOverlapAxis * minOverlap;

	if (glm::normalize(minOverlapAxis).y >= 0.9)
	{
		Logger::log("standing on Object");
		movement.y = 0;
		onOtherObject = true;
		collisionResult->onTop = true;
	}
	else if (glm::normalize(minOverlapAxis).y >= 0 && object->getCollissionBoxType() == CollissionBoxType::prism)
	{
		MinimumTranslationVector = MinimumTranslationVector * glm::vec3(0, 1, 0);
	}


	this->position += MinimumTranslationVector;

	collisionResult->collided = true;
	collisionResult->collidedWith = object;
	collisionResult->MinimumTranslationVector = MinimumTranslationVector;

	
	return true;

}

void Object::calculationBeforeFrame()
{
	
}

void Object::calculationAfterFrame()
{

}

glm::vec3 Object::calculateDimensions()
{
	dimensions = model->getDimension();
	boundingboxdimensions = model->getBoundingBoxDimension();
	Logger::log(printObject()+" Dimensions: x: " + std::to_string(dimensions.x) + " y: " + std::to_string(dimensions.y) + " z: " + std::to_string(dimensions.z));
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




	if (this->getCollissionBoxType() == CollissionBoxType::cube)
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
	else if (this->getCollissionBoxType() == CollissionBoxType::prism)
	{
		float cosW = cos((-rotation.y-90) * 3.14 / 180);
		float sinW = sin((-rotation.y-90) * 3.14 / 180);
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


	return cubePoints;
}

std::vector<glm::vec3> Object::calculateCollisionNormals()
{
	std::vector<glm::vec3> normals;
	glm::vec3 normal;

	if (this->getCollissionBoxType() == CollissionBoxType::cube)
	{
		normal = (glm::normalize(cubePoints[1] - cubePoints[0]));
		normals.push_back(normal);

		normal = (glm::normalize(cubePoints[2] - cubePoints[1]));
		normals.push_back(normal);

		normal = (glm::normalize(cubePoints[7] - cubePoints[0]));
		normals.push_back(normal);
	}
	else if (this->getCollissionBoxType() == CollissionBoxType::prism)
	{
		normal = (glm::normalize(cubePoints[1] - cubePoints[0]));
		normals.push_back(normal);

		normal = (glm::normalize(cubePoints[2] - cubePoints[1]));
		normals.push_back(normal);

		normal = (glm::normalize(cubePoints[5] - cubePoints[0]));
		normals.push_back(normal);

		normal = (glm::normalize( glm::cross( glm::normalize(cubePoints[5] - cubePoints[4]), glm::normalize(cubePoints[4] - cubePoints[2]) ) ));
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

bool Object::checkBoundaries(Object* map)
{
	bool outOfBounds = false;
	glm::vec3 newPosition = position + movement;

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
	if (newPosition.y / 2 < map->getPosition().y - 1 )
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

	if (outOfBounds)
	{
		Logger::log("Object out of Bounds: " + printObject());
	}
	return outOfBounds;
}

void Object::fall(float32 deltaTime, std::vector<Object*> objects)
{
	
	if (position.y > 0 && !onOtherObject)
	{
		Logger::log("Object: " +printObject()+" is falling");
		movement.y -= 9.81 * 200 * deltaTime;		
	}

	/*if(position.y <= 0)
	{
		movement.y = 0;
		position.y = 0;
	}*/

	
}

void Object::move(float32 deltaTime)
{
	position += movement;

	center = position;
	center.y += dimensions.y / 2;

	//if the objects moves onOtherObject should be calculated new
	if (movement != glm::vec3(0, 0, 0))
	{
		onOtherObject = false;
	}
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
	this->model->render();
}

void Object::registerHit()
{
	health = 0;
}

void Object::setHealth(float32 newHealth)
{
	health = newHealth;
}

float32 Object::getHealth()
{
	return health;
}

std::string Object::printObject()
{
	return "[" + std::to_string(getNumber()) + "|"+ getName() + "|" + std::to_string(getType()) + "]";
}

void Object::setCollissionBoxType(CollissionBoxType collissionBoxType)
{
	this->collissionBoxType = collissionBoxType;
}

CollissionBoxType Object::getCollissionBoxType()
{
	return collissionBoxType;
}

CollissionBoxType Object::convertStringToCollissionBoxType(std::string ollissionBoxTypeAsString)
{
	auto it = collissionBoxTypeTable.find(ollissionBoxTypeAsString);
	if (it != collissionBoxTypeTable.end()) {
		return it->second;
	}
}