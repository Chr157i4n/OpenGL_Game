#pragma once
#include "Model.h"
#include "Shader.h"
#include "libs/glm/glm.hpp"
#include "libs/glm/ext/matrix_transform.hpp"
#include "libs/glm/gtc/matrix_transform.hpp"

enum ObjectType {
	Object_Player = 0,
	Object_Bot = 1,
	Object_Entity = 2,
	Object_Environment = 3,
	Object_Bullet = 3,
};

class Object
{
public:
	Object(Shader* shader, std::string modelFileName);

	Shader* getShader();

	void bindShader();

	void unbindShader();

	bool checkCollision(Object* object, glm::vec3 newPosition);

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

	void setMovement(glm::vec3 newMovement);

	glm::vec3 getMovement();

	void calculateDimensions();

	glm::vec3 getDimensions();

	void setObjectType(ObjectType newObjectType);

	ObjectType getObjectType();

	std::string getName();

	void render();

protected:
	
	glm::vec3 position;			//x, y, z		rechts, oben, vorne
	glm::vec3 rotation;			//yaw, pitch, roll	gieren (360°), Steigung (360° | +-90°), Rollen (360°)
	glm::vec3 movement;	//x, y, z
	glm::vec3 dimensions;		//x, y, z

	Model* model = nullptr;
	Shader* shader = nullptr;
	ObjectType objectType;
	std::string name="";
};

