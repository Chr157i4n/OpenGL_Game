#pragma once

#include <math.h>
#include <unordered_map>

#include "libs/glm/glm.hpp"
#include "libs/glm/ext/matrix_transform.hpp"
#include "libs/glm/gtc/matrix_transform.hpp"

#include "Model.h"
#include "Shader.h"
#include "Logger.h"



enum ObjectType {
	Object_Player,
	Object_Bot,
	Object_Entity,
	Object_Environment,
	Object_Bullet
};

std::unordered_map<std::string, ObjectType> const objectTypeTable =
{
	{"Bot",ObjectType::Object_Bot},
	{"Bullet",ObjectType::Object_Bullet},
	{"Entity",ObjectType::Object_Entity},
	{"Environment",ObjectType::Object_Environment},
	{"Player",ObjectType::Object_Player}
};

struct CollisionResult
{
	bool collided = false;
	glm::vec3 MinimumTranslationVector = glm::vec3(0,0,0);
};

class Object
{
public:
	Object(Shader* shader, std::string modelFileName);

	Shader* getShader();

	void bindShader();

	void unbindShader();


	CollisionResult checkCollision(std::vector<Object*> objects);

	bool checkCollision_AABB(Object* object);

	bool checkCollision_SAT(Object* object, CollisionResult* collisionResult);

	glm::vec3 calculateDimensions();

	std::vector<glm::vec2> calculateRectPoints();

	std::vector<glm::vec3> calculateCubePoints();

	std::vector<glm::vec3> calculateCubeNormals();

	std::vector<glm::vec2> getRectPoints();

	std::vector<glm::vec3> getCubePoints();

	std::vector<glm::vec3> getCubeNormals();

	bool checkBoundaries(Object* map, glm::vec3 newPosition);

	//deltaTime in seconds
	void fall(float32 deltaTime, std::vector<Object*> objects);

	//deltaTime in seconds
	void move(float32 deltaTime);

	void loadModel(std::string modelFileName);

	Model* getModel();

	void setPosition(glm::vec3 newPosition);

	glm::vec3 getPosition();

	void setRotation(glm::vec3 newRotation);

	glm::vec3 getRotation();

	void setScale(glm::vec3 newScale);

	glm::vec3 getScale();

	void setMovement(glm::vec3 newMovement);

	glm::vec3 getMovement();

	glm::vec3 getDimensions();

	glm::vec3 getBoundingBoxDimensions();

	void setType(ObjectType newObjectType);

	ObjectType getType();

	static ObjectType convertStringToType(std::string objectTypeAsString);

	void setName(std::string newName);

	std::string getName();

	void setNumber(int32 newNumber);

	int32 getNumber();

	void render();

	void registerHit();

	void setHealth(float32 newHealth);

	float32 getHealth();

	std::string printObject();

protected:

	glm::vec3 position;						//x, y, z
	glm::vec3 center;						//x, y, z
	glm::vec3 rotation;						//yaw, pitch, roll	
	glm::vec3 scale;
	glm::vec3 movement;						//x, y, z
	glm::vec3 dimensions;					//x, y, z
	glm::vec3 boundingboxdimensions;		//x, y, z

	std::vector<glm::vec2> rectPoints;
	std::vector<glm::vec3> cubePoints;
	std::vector<glm::vec3> cubeNormals;

	Model* model = nullptr;
	Shader* shader = nullptr;
	ObjectType type;
	std::string name = "";
	int32 number;
	float32 health = 100;
	bool onOtherObject=false;
};

