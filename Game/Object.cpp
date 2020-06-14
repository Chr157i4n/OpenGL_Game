#include "Object.h"
#include "UI.h"
#include <algorithm>
#include "Logger.h"
#include "libs/glm/gtx/rotate_vector.hpp"

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
	calculateDimensions();
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

bool Object::checkCollisionXY(Object* object, glm::vec3 newPosition)
{

	if (checkCollisionXY_AABB(object, newPosition))
	{
		return checkCollisionXY_SAT(object, newPosition);
	}
	else
	{
		return false;
	}

}

bool Object::checkCollisionXY_AABB(Object* object, glm::vec3 newPosition)
{
	float playerMaxX = newPosition.x + dimensions.x / 2;
	float playerMinX = newPosition.x - dimensions.x / 2;

	float playerMaxY = newPosition.y + dimensions.y / 2;
	float playerMinY = newPosition.y - dimensions.y / 2;

	float playerMaxZ = newPosition.z + dimensions.z / 2;
	float playerMinZ = newPosition.z - dimensions.z / 2;

	glm::vec3 objectPosition = object->getPosition();
	glm::vec3 objectBoundingBoxDimension = object->getBoundingBoxDimensions();
	glm::vec3 objectRotation = object->getRotation();

	float objectMinX = objectPosition.x - (objectBoundingBoxDimension.x / 2);
	float objectMaxX = objectPosition.x + (objectBoundingBoxDimension.x / 2);

	float objectMinY = objectPosition.y - (objectBoundingBoxDimension.y / 2);
	float objectMaxY = objectPosition.y + (objectBoundingBoxDimension.y / 2);

	float objectMinZ = objectPosition.z - (objectBoundingBoxDimension.z / 2);
	float objectMaxZ = objectPosition.z + (objectBoundingBoxDimension.z / 2);


#pragma region UI output

	//#define DEBUG_AAB

#ifdef DEBUG_AAB

	std::string UI_Text = "Min: ";

	std::stringstream xminss, yminss, zminss;
	xminss << std::fixed << std::setprecision(1) << objectMinX;
	std::string xMinString = xminss.str();

	yminss << std::fixed << std::setprecision(1) << objectMinY;
	std::string yMinString = yminss.str();

	zminss << std::fixed << std::setprecision(1) << objectMinZ;
	std::string zMinString = zminss.str();

	UI_Text += "x " + xMinString + ", ";
	UI_Text += "y " + yMinString + ", ";
	UI_Text += "z " + zMinString;

	UI_Element* objectmin = new UI_Element();
	objectmin->posX = UI::getWidth() - 300;
	objectmin->posY = 50;
	objectmin->text = UI_Text;
	UI::addElement(objectmin);


	UI_Text = "Max: ";

	std::stringstream xmaxss, ymaxss, zmaxss;
	xmaxss << std::fixed << std::setprecision(1) << objectMaxX;
	std::string xMaxString = xmaxss.str();

	ymaxss << std::fixed << std::setprecision(1) << objectMaxY;
	std::string yMaxString = ymaxss.str();

	zmaxss << std::fixed << std::setprecision(1) << objectMaxZ;
	std::string zMaxString = zmaxss.str();

	UI_Text += "x " + xMaxString + ", ";
	UI_Text += "y " + yMaxString + ", ";
	UI_Text += "z " + zMaxString;

	UI_Element* objectmax = new UI_Element();
	objectmax->posX = UI::getWidth() - 300;
	objectmax->posY = 80;
	objectmax->text = UI_Text;
	UI::addElement(objectmax);

#endif // DEBUG_AAB
#pragma endregion


	// X - Achse -> von positiver Richtung
	if (playerMaxX < objectMinX)
		return false;

	// X - Achse -> von negativer Richtung
	if (playerMinX > objectMaxX)
		return false;

	// Y - Achse -> von positiver Richtung
	if (playerMaxY < objectMinY)
		return false;

	// Y - Achse -> von negativer Richtung
	if (playerMinY > objectMaxY)
		return false;

	// Z - Achse von positiver Richtung
	if (playerMaxZ < objectMinZ)
		return false;

	// Z - Achse -> von negativer Richtung
	if (playerMinZ > objectMaxZ)
		return false;

	Logger::log("AABB collision between: [MFN: " + getName() + ",OT: " + std::to_string(getType()) + ",ON: " + std::to_string(getNumber()) + "]  and  [MFN: " + object->getName() + ",OT: " + std::to_string(object->getType()) + ",ON: " + std::to_string(object->getNumber()) + "]");
	return true;
}

bool Object::checkCollisionXY_SAT(Object* object, glm::vec3 newPosition)
{
	Object object1 = *this;
	Object object2 = *object;

	object1.setPosition(newPosition);


	std::vector<std::vector<glm::vec2>> RectPoints;

	RectPoints.push_back(getRectPoints(&object1));
	RectPoints.push_back(getRectPoints(&object2));

	std::vector<glm::vec2> axes;

	//get all axes of both shapes
	for (int shape = 0; shape < 2; shape++)
	{
		// loop over the vertices(-> edges -> axis) of the first polygon
		for (auto i = 0u; i < RectPoints[shape].size() + 0; ++i)
		{
			// calculate the normal vector of the current edge
						// this is the axis will we check in this loop
			auto current = RectPoints[shape][i];
			auto next = RectPoints[shape][(i + 1) % RectPoints[shape].size()];
			auto edge = next - current;

			glm::vec2 axis;
			axis.x = -edge.y;
			axis.y = edge.x;

			axes.push_back(axis);
		}

	}


	for (glm::vec2 axis : axes)
	{
		// loop over all vertices of both polygons and project them
					// onto the axis. We are only interested in max/min projections
		auto aMaxProj = -std::numeric_limits<float>::infinity();
		auto aMinProj = std::numeric_limits<float>::infinity();
		auto bMaxProj = -std::numeric_limits<float>::infinity();
		auto bMinProj = std::numeric_limits<float>::infinity();
		for (const auto& v : RectPoints[0]) {
			auto proj = glm::dot(axis, v);
			if (proj < aMinProj) aMinProj = proj;
			if (proj > aMaxProj) aMaxProj = proj;
		}

		for (const auto& v : RectPoints[1]) {
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
	}


	// at this point, we have checked all axis but found no separating axis
	// which means that the polygons must intersect.
	Logger::log("SAT collision between: [MFN: " + getName() + ",OT: " + std::to_string(getType()) + ",ON: "+ std::to_string(getNumber()) + "]  and  [MFN: " + object->getName() + ",OT: " + std::to_string(object->getType()) + ",ON: " + std::to_string(object->getNumber()) + "]");
	return true;

}

std::vector<glm::vec2> Object::getRectPoints(Object* object)
{
	std::vector<glm::vec2> rectPoints;

	float cosW = cos(-object->getRotation().y * 3.14 / 180);
	float sinW = sin(-object->getRotation().y * 3.14 / 180);
	glm::mat2 drehmatrix = glm::mat2(cosW, sinW, -sinW, cosW);
	//Punkt

	glm::vec2 newPoint = glm::vec2(object->getDimensions().x / 2, object->getDimensions().z / 2);

	glm::vec2 newRotatedPoint;
	newRotatedPoint = drehmatrix * newPoint;

	newRotatedPoint.x += object->getPosition().x;
	newRotatedPoint.y += object->getPosition().z;

	rectPoints.push_back(newRotatedPoint);

	//Punkt
	newPoint = glm::vec2(-object->getDimensions().x / 2, object->getDimensions().z / 2);

	newRotatedPoint = drehmatrix * newPoint;

	newRotatedPoint.x += object->getPosition().x;
	newRotatedPoint.y += object->getPosition().z;

	rectPoints.push_back(newRotatedPoint);

	//Punkt
	newPoint = glm::vec2(-object->getDimensions().x / 2, -object->getDimensions().z / 2);

	
	newRotatedPoint = drehmatrix * newPoint;

	newRotatedPoint.x += object->getPosition().x;
	newRotatedPoint.y += object->getPosition().z;

	rectPoints.push_back(newRotatedPoint);

	//Punkt
	newPoint = glm::vec2(object->getDimensions().x / 2, -object->getDimensions().z / 2);

	newRotatedPoint = drehmatrix * newPoint;

	newRotatedPoint.x += object->getPosition().x;
	newRotatedPoint.y += object->getPosition().z;

	rectPoints.push_back(newRotatedPoint);

	return rectPoints;
}

bool Object::checkBoundaries(Object* map, glm::vec3 newPosition)
{
	// X - Achse
	if (newPosition.x - dimensions.x / 2 < map->getPosition().x - map->getDimensions().x / 2)
	{
		Logger::log("Object out of Bounds: [MFN: " + getName() + ",OT: " + std::to_string(getType()) + ",ON: " + std::to_string(getNumber()) + "]");
		return true;
	}

	if (newPosition.x + dimensions.x / 2 > map->getPosition().x + map->getDimensions().x / 2)
	{
		Logger::log("Object out of Bounds: [MFN: " + getName() + ",OT: " + std::to_string(getType()) + ",ON: " + std::to_string(getNumber()) + "]"); 
		return true;
	}

	/*
	// Y - Achse
	if (newPosition.y - dimensions.y / 2 < map->getPosition().y - map->getDimensions().y / 2)
	{
		Logger::log("Player out of bounds. Obejctposition: x: " + std::to_string(position.x) + " y: " + std::to_string(position.y) + " z: " + std::to_string(position.z));
		return true;
	}

	if (newPosition.y + dimensions.y / 2 > map->getPosition().y + map->getDimensions().y / 2)
	{
		Logger::log("Player out of bounds. Obejctposition: x: " + std::to_string(position.x) + " y: " + std::to_string(position.y) + " z: " + std::to_string(position.z));
		return true;
	}
	*/

	// Z - Achse
	if (newPosition.z - dimensions.z / 2 < map->getPosition().z - map->getDimensions().z / 2)
	{
		Logger::log("Object out of Bounds: [MFN: " + getName() + ",OT: " + std::to_string(getType()) + ",ON: " + std::to_string(getNumber()) + "]");
		return true;
	}

	if (newPosition.z + dimensions.z / 2 > map->getPosition().z + map->getDimensions().z / 2)
	{
		Logger::log("Object out of Bounds: [MFN: " + getName() + ",OT: " + std::to_string(getType()) + ",ON: " + std::to_string(getNumber()) + "]");
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

void Object::calculateDimensions()
{
	dimensions = model->getDimension();
	boundingboxdimensions = model->getBoundingBoxDimension();
	Logger::log("Object Dimension x: " + std::to_string(dimensions.x) + " y: " + std::to_string(dimensions.y) + " z: " + std::to_string(dimensions.z));
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
