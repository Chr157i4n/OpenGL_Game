#pragma once
#include "defines.h"
#include "Types.h"

#include <algorithm>
#include <chrono>
#include <limits>
#include <memory>
#include <string>
#include <vector>

#include <glm/glm.hpp>

/// <summary>
/// Shared, renderer-independent simulation core for every object in the game world.
///
/// The core knows nothing about OpenGL, shaders, model files or the network. Client
/// and server derive from it and add their own rendering/networking layer. All time
/// dependent behavior is passed in explicitly (delta in milliseconds), so the core
/// can be unit tested without a running game loop.
/// </summary>
class ObjectCore
{
public:

	explicit ObjectCore(glm::vec3 initialDimensions = glm::vec3(2, 2, 2));

	virtual ~ObjectCore() = default;

	bool  intersectWithRay(glm::vec3 rayOrigin, glm::vec3 rayDirection) const;

	float getDistance(std::shared_ptr<ObjectCore> other) const;

	void markObject();
	void markObject(float32 durationMs);
	bool isMarked() const;

	virtual void interact();

	/// Collides this object against every enabled object in `world`.
	CollisionResult checkCollision(const std::vector<std::shared_ptr<ObjectCore>>& world);

	bool checkCollision_AABB(std::shared_ptr<ObjectCore> other) const;

	bool checkCollision_SAT(std::shared_ptr<ObjectCore> other, CollisionResult* collisionResult);

	virtual void reactToCollision(CollisionResult collisionResult, float32 deltaMs);

	void calculationBeforeFrame();

	void calculationAfterFrame();

	/// Sets the object size and rebuilds the collision geometry.
	void setDimensions(glm::vec3 newDimensions);

	glm::vec3 calculateDimensions();

	std::vector<glm::vec2> calculateRectPoints();

	std::vector<glm::vec3> calculateCollisionPoints();

	std::vector<glm::vec3> calculateCollisionNormals();

	std::vector<glm::vec2> getRectPoints() const;

	std::vector<glm::vec3> getCubePoints() const;

	std::vector<glm::vec3> getCubeNormals() const;

	/// Returns true if the object would leave the map bounds on the next step.
	/// `map` is the map object whose position and dimensions describe the bounds.
	bool checkBoundaries(std::shared_ptr<ObjectCore> map, float32 deltaMs);

	virtual void fall(float32 deltaMs, glm::vec3 gravity);

	virtual void move(float32 deltaMs, std::shared_ptr<ObjectCore> map);

	void setPosition(glm::vec3 newPosition);

	glm::vec3 getPosition() const;

	virtual void setRotation(glm::vec3 newRotation);

	glm::vec3 getRotation() const;

	glm::vec3 getCenter() const
	{
		return center;
	}

	void setScale(glm::vec3 newScale);

	glm::vec3 getScale() const;

	void setMovement(glm::vec3 newMovement);

	glm::vec3 getMovement() const;

	glm::vec3 getDimensions() const;

	glm::vec3 getBoundingBoxDimensions() const;

	void setType(ObjectType newObjectType);

 ObjectType getType() const;

	static ObjectType convertStringToType(std::string objectTypeAsString);

	void setName(std::string newName);

	std::string getName() const;

	void setNumber(int32 newNumber);

	int32 getNumber() const;

	virtual void registerHit();

	void setHealth(float32 newHealth);

	float32 getHealth() const;

	bool isAlive() const
	{
		return getHealth() > 0;
	}

	virtual void addToHealth(float32 addHealth);

	std::string printObject() const;

	std::string printPosition() const;

	void setCollisionBoxType(CollisionBoxType newCollisionBoxType);

	CollisionBoxType getCollisionBoxType() const;

	static CollisionBoxType convertStringToCollisionBoxType(std::string collisionBoxTypeAsString);

	void setGravity(bool enable);

	bool getGravity() const;

	bool isGettingDamaged() const;

	void enable()
	{
		enabled = true;
	}

	void disable()
	{
		enabled = false;
	}

	bool getEnabled() const
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

	void setOnOtherObject(bool standing)
	{
		onOtherObject = standing;
	}

	int getNetworkID() const
	{
		return networkID;
	}

	void setNetworkID(int newNetworkID)
	{
		networkID = newNetworkID;
	}

	std::string getNetworkIDString() const
	{
		std::string answer = "";

		if (this->getNetworkID() < 100) answer += "0";
		if (this->getNetworkID() < 10) answer += "0";

		answer += std::to_string(this->getNetworkID());
		return answer;
	}

protected:

	virtual void updateCollisionGeometry();

	glm::vec3 position;				//x, y, z
	glm::vec3 center;					//x, y, z
	glm::vec3 rotation;					//yaw, pitch, roll
	glm::vec3 scale;
	glm::vec3 movement;					//x, y, z
	glm::vec3 dimensions;				//x, y, z
	glm::vec3 boundingboxdimensions;	//x, y, z

	std::vector<glm::vec2> rectPoints;
	std::vector<glm::vec3> cubePoints;
	std::vector<glm::vec3> cubeNormals;

	ObjectType type = ObjectType::Object_Entity;
	CollisionBoxType collisionBoxType = CollisionBoxType::none;
	std::string name = "";
	int32 number = 0;
	float32 health = 100;
	bool onOtherObject = false;
	bool gravity = true;
	bool enabled = true;

	std::chrono::system_clock::time_point lastHitTimestamp = std::chrono::system_clock::now() - std::chrono::hours(1);
	std::chrono::system_clock::time_point lastMarkTimestamp = std::chrono::system_clock::now() - std::chrono::hours(1);
	float32 markDuration = 1000;

	int networkID = -1;
};