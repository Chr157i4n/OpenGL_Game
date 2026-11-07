#pragma once

#include "defines.h"

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <glm/glm.hpp>

// Forward declaration: the actual Object class lives in Game/ or GameServer/.
class Object;

/// <summary>
/// enum for different type of an object. It is used as flags, so the Player, has the flag "Object_Player" and "Object_Character".
/// an enum has the doubled value of the previous one, so that you have no overlap, if you add some flags
/// </summary>
enum ObjectType {
	Object_Player		= 1,
	Object_NPC			= 2,
	Object_Character	= 4,
	Object_Entity		= 8,
	Object_Interactable	= 16,
	Object_Environment	= 32,
	Object_Bullet		= 64,
};

/// <summary>
/// operator for using the enum ObjectType as flags
/// </summary>
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
