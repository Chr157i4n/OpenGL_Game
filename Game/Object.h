#pragma once
#include "Model.h"
#include "Shader.h"
#include "libs/glm/glm.hpp"
#include "libs/glm/ext/matrix_transform.hpp"
#include "libs/glm/gtc/matrix_transform.hpp"
#include <math.h>
#include <unordered_map>

enum ObjectType {
	Object_Player = 0,
	Object_Bot = 1,
	Object_Entity = 2,
	Object_Environment = 3,
	Object_Bullet = 4,
};

std::unordered_map<std::string, ObjectType> const objectTypeTable =
{
	{"Bot",ObjectType::Object_Bot},
	{"Bullet",ObjectType::Object_Bullet},
	{"Entity",ObjectType::Object_Entity},
	{"Environment",ObjectType::Object_Environment},
	{"Player",ObjectType::Object_Player}
};

class Object
{
public:
	Object(Shader* shader, std::string modelFileName);

	Shader* getShader();

	void bindShader();

	void unbindShader();

	bool checkCollisionXY(Object* object, glm::vec3 newPosition);

	bool checkCollisionXY_AABB(Object* object, glm::vec3 newPosition);

	bool checkCollisionXY_SAT(Object* object, glm::vec3 newPosition);

	std::vector<glm::vec2> getRectPoints(Object* object);

	bool checkBoundaries(Object* map, glm::vec3 newPosition);

	//deltaTime in seconds
	void fall(float32 deltaTime);

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

	void calculateDimensions();

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

protected:
	
	glm::vec3 position;						//x, y, z		rechts, oben, vorne
	glm::vec3 rotation;						//yaw, pitch, roll	gieren (360°), Steigung (360° | +-90°), Rollen (360°)
	glm::vec3 scale;
	glm::vec3 movement;						//x, y, z
	glm::vec3 dimensions;					//x, y, z
	glm::vec3 boundingboxdimensions;		//x, y, z

	Model* model = nullptr;
	Shader* shader = nullptr;
	ObjectType type;
	std::string name="";
	int32 number;
};

