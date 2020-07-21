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
	Object_Player		= 1,
	Object_NPC			= 2,
	Object_Character	= 4,
	Object_Entity		= 8,
	Object_Environment	= 16,
	Object_Bullet		= 32,
};

inline ObjectType operator|(ObjectType a, ObjectType b)
{
	return static_cast<ObjectType>(static_cast<int>(a) | static_cast<int>(b));
}

std::unordered_map<std::string, ObjectType> const objectTypeTable =
{
	{"Bot",ObjectType::Object_NPC},
	{"Bullet",ObjectType::Object_Bullet},
	{"Entity",ObjectType::Object_Entity},
	{"Environment",ObjectType::Object_Environment},
	{"Player",ObjectType::Object_Player}
};

enum CollisionBoxType {
	none,
	cube,
	prism,
};

std::unordered_map<std::string, CollisionBoxType> const CollisionBoxTypeTable =
{
	{"none",CollisionBoxType::none},
	{"cube",CollisionBoxType::cube},
	{"prism",CollisionBoxType::prism},
};

class Object;

struct CollidedObject
{
	std::shared_ptr<Object> object = nullptr;
	bool onTop = false;
	glm::vec3 MinimumTranslationVector = glm::vec3(0, 0, 0);
};

struct CollisionResult
{
	bool collided = false;
	std::vector<CollidedObject*> collidedObjectList;	
};



class Object
{
public:


	Object(Shader* shader, std::string modelFileName);
	

	Shader* getShader();

	void bindShader();

	void unbindShader();


	CollisionResult checkCollision(std::vector< std::shared_ptr<Object>> objects);

	bool checkCollision_AABB(std::shared_ptr < Object> object);

	bool checkCollision_SAT(std::shared_ptr < Object> object, CollisionResult* collisionResult);

	void calculationBeforeFrame();

	void calculationAfterFrame();

	glm::vec3 calculateDimensions();

	std::vector<glm::vec2> calculateRectPoints();

	std::vector<glm::vec3> calculateCollisionPoints();

	std::vector<glm::vec3> calculateCollisionNormals();

	std::vector<glm::vec2> getRectPoints();

	std::vector<glm::vec3> getCubePoints();

	std::vector<glm::vec3> getCubeNormals();

	bool checkBoundaries(std::shared_ptr<Object> map);

	//deltaTime in seconds
	virtual void fall(float32 deltaTime);

	//deltaTime in seconds
	void move(float32 deltaTime, std::shared_ptr < Object> map);

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

	void renderShadowMap();

	void registerHit();

	void setHealth(float32 newHealth);

	float32 getHealth();

	std::string printObject();

	std::string printPosition();

	void setCollisionBoxType(CollisionBoxType collisionBoxType);

	CollisionBoxType getCollisionBoxType();

	static CollisionBoxType convertStringToCollisionBoxType(std::string collissionBoxTypeAsString);

	void setGravity(bool enable);

	bool getGravity();

	void setModel(Model* newModel);

	Model* getModel();

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

	int modelIndex = -1;
	Shader* shader = nullptr;
	Model* model = nullptr;
	ObjectType type;
	CollisionBoxType collisionBoxType;
	std::string name = "";
	int32 number;
	float32 health = 100;
	bool onOtherObject=false;
	bool gravity = true;
};

