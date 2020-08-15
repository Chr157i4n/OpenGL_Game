#pragma once
#include "defines.h"

#include <math.h>
#include <unordered_map>
#include "libs/glm/glm.hpp"
#include "libs/glm/ext/matrix_transform.hpp"
#include "libs/glm/gtc/matrix_transform.hpp"
#include "libs/glm/gtx/rotate_vector.hpp"
#include <algorithm>
#include <memory>
#include <chrono>
#include <string>

#include "Model.h"



/// <summary>
/// enum for different type of an object. It is used as flags, so the Player, has the flag "Object_Player" and "Object_Character".
/// an enum has the doubled value of the previous one, so that you have no overlap, if you add some flags
/// </summary>
enum ObjectType {
	Object_Player		= 1,
	Object_NPC			= 2,
	Object_Character	= 4,
	Object_Entity		= 8,
	Object_Environment	= 16,
	Object_Bullet		= 32,
};

/// <summary>
/// operator for using the enum ObjectType as flags
/// </summary>
/// <param name="a">first flag</param>
/// <param name="b">second flag</param>
/// <returns></returns>
inline ObjectType operator|(ObjectType a, ObjectType b)
{
	return static_cast<ObjectType>(static_cast<int>(a) | static_cast<int>(b));
}

/// <summary>
/// map to get the objecttype, when loading a map and getting the objecttype as string
/// </summary>
std::unordered_map<std::string, ObjectType> const objectTypeTable =
{
	{"Bot",ObjectType::Object_NPC},
	{"Bullet",ObjectType::Object_Bullet},
	{"Entity",ObjectType::Object_Entity},
	{"Environment",ObjectType::Object_Environment},
	{"Player",ObjectType::Object_Player}
};

/// <summary>
/// enum for the type of collision box the object should use
/// </summary>
enum CollisionBoxType {
	none,
	cube,
	prism,
};

/// <summary>
///  map to get the CollisionBoxType, when loading a map and getting the CollisionBoxType as string
/// </summary>
std::unordered_map<std::string, CollisionBoxType> const CollisionBoxTypeTable =
{
	{"none",CollisionBoxType::none},
	{"cube",CollisionBoxType::cube},
	{"prism",CollisionBoxType::prism},
};

//forward declaration of the class Object
class Object;

/// <summary>
/// struct for each object the object collided
/// </summary>
struct CollidedObject
{
	std::shared_ptr<Object> object = nullptr;
	bool onTop = false;
	glm::vec3 MinimumTranslationVector = glm::vec3(0, 0, 0);
	glm::vec3 movementBeforeCollision = glm::vec3(0, 0, 0);
};

/// <summary>
/// CollisionResult to save with which Objects the object collided
/// </summary>
struct CollisionResult
{
	bool collided = false;
	glm::vec3 movementBeforeCollision = glm::vec3(0, 0, 0);
	std::vector<CollidedObject*> collidedObjectList;	
};


/// <summary>
/// Base class for every object in the game world
/// </summary>
class Object
{
public:

	Object(std::string modelFileName);


	bool  intersectWithRay(glm::vec3 rayOrigin, glm::vec3 rayDirection);

	float getDistance(std::shared_ptr<Object> object);

	void markObject();

	void interact();

	CollisionResult checkCollision();

	bool checkCollision_AABB(std::shared_ptr < Object> object);

	bool checkCollision_SAT(std::shared_ptr < Object> object, CollisionResult* collisionResult);

	virtual void reactToCollision(CollisionResult collisionResult);

	void calculationBeforeFrame();

	void calculationAfterFrame();

	glm::vec3 calculateDimensions();

	std::vector<glm::vec2> calculateRectPoints();

	std::vector<glm::vec3> calculateCollisionPoints();

	std::vector<glm::vec3> calculateCollisionNormals();

	std::vector<glm::vec2> getRectPoints();

	std::vector<glm::vec3> getCubePoints();

	std::vector<glm::vec3> getCubeNormals();

	bool checkBoundaries();

	virtual void fall();

	virtual void move();

	void setPosition(glm::vec3 newPosition);

	glm::vec3 getPosition();

	virtual void setRotation(glm::vec3 newRotation);

	glm::vec3 getRotation();

	glm::vec3 getCenter()
	{
		return center;
	}

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

	virtual void registerHit();

	void setHealth(float32 newHealth);

	float32 getHealth();

	bool isAlive() {
		return (this->getHealth() > 0) ? true : false;
	}

	virtual void addToHealth(float32 addHealth);

	std::string printObject();

	std::string printPosition();

	void setCollisionBoxType(CollisionBoxType collisionBoxType);

	CollisionBoxType getCollisionBoxType();

	static CollisionBoxType convertStringToCollisionBoxType(std::string collissionBoxTypeAsString);

	void setGravity(bool enable);

	bool getGravity();

	void setModel(Model* newModel);

	void setModel(std::string newModelFileName);

	Model* getModel();

	bool isGettingDamaged();

	void enable()
	{
		enabled = true;
	}

	void disable()
	{
		enabled = false;
	}

	bool getEnabled()
	{
		return enabled;
	}

	void toggleEnabled()
	{
		enabled = !enabled;
	}

	void setEnabled(bool enable)
	{
		enabled = enable;
	}

	int getNetworkID()
	{
		return networkID;
	}

	void setNetworkID(int newNetworkID)
	{
		networkID = newNetworkID;
	}

	std::string getNetworkIDString()
	{
		std::string answer = "";

		if (this->getNetworkID() < 100) answer += "0";
		if (this->getNetworkID() < 10) answer += "0";

		answer += std::to_string(this->getNetworkID());
		return answer;
	}

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
	Model* model = nullptr;
	ObjectType type;
	CollisionBoxType collisionBoxType;
	std::string name = "";
	int32 number;
	float32 health = 100;
	bool onOtherObject=false;
	bool gravity = true;
	bool enabled = true;
	glm::vec2 textureFlow = glm::vec2(0, 0);

	std::chrono::system_clock::time_point lastHitTimestamp = std::chrono::system_clock::now() - std::chrono::hours(1);

	int envCubeMap=-1, envCubeMapFrameBuffer=-1;

	int networkID = -1;
};

