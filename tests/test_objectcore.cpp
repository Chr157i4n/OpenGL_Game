#include <catch2/catch_test_macros.hpp>

#include <vector>

#include "ObjectCore.h"

TEST_CASE("ObjectCore starts with sane default dimensions", "[ObjectCore]")
{
	ObjectCore object;

	REQUIRE(object.getDimensions() == glm::vec3(2, 2, 2));
	REQUIRE(object.getBoundingBoxDimensions() == glm::vec3(4, 4, 4));
	REQUIRE(object.getCenter() == glm::vec3(0, 1, 0));
	REQUIRE(object.getType() == ObjectType::Object_Entity);
	REQUIRE(object.getGravity());
	REQUIRE(object.getEnabled());
}

TEST_CASE("ObjectCore rebuilds collision geometry when the box type is set", "[ObjectCore]")
{
	ObjectCore object(glm::vec3(4, 3, 5));
	object.setCollisionBoxType(CollisionBoxType::cube);

	REQUIRE(object.getCubePoints().size() == 8);
	REQUIRE(object.getCubeNormals().size() == 3);
	REQUIRE(object.getRectPoints().size() == 4);
}

TEST_CASE("ObjectCore keeps bounding geometry for none collision boxes", "[ObjectCore]")
{
	ObjectCore object(glm::vec3(2, 2, 2));
	object.setCollisionBoxType(CollisionBoxType::none);

	REQUIRE(object.getCubePoints().size() == 8);
	REQUIRE(object.getCubeNormals().empty());
}

TEST_CASE("ObjectCore ray intersection finds objects in front of the ray", "[ObjectCore]")
{
	ObjectCore object(glm::vec3(2, 2, 2));
	object.setCollisionBoxType(CollisionBoxType::cube);

	REQUIRE(object.intersectWithRay(glm::vec3(0, -1, 0), glm::vec3(0, 1, 0)));
	REQUIRE(!object.intersectWithRay(glm::vec3(0, -1, 10), glm::vec3(0, 1, 0)));
}

TEST_CASE("ObjectCore move stops movement at map boundaries", "[ObjectCore]")
{
	auto map = std::make_shared<ObjectCore>(glm::vec3(100, 200, 100));
	map->setPosition(glm::vec3(0, 0, 0));

	ObjectCore object(glm::vec3(2, 2, 2));
	object.setPosition(glm::vec3(0, 50, 0));
	object.setMovement(glm::vec3(100, 0, 0));

	REQUIRE(object.checkBoundaries(map, 1000.0f));

	object.move(1000.0f, map);

	REQUIRE(object.getMovement().x == 0.0f);
	REQUIRE(object.getPosition().x == 0.0f);
}

TEST_CASE("ObjectCore detects overlapping SAT collision and applies movement translation", "[ObjectCore]")
{
	ObjectCore first(glm::vec3(2, 2, 2));
	first.setPosition(glm::vec3(0, 0, 0));
	first.setCollisionBoxType(CollisionBoxType::cube);
	first.setMovement(glm::vec3(1, 0, 0));

	ObjectCore second(glm::vec3(2, 2, 2));
	second.setPosition(glm::vec3(1.5f, 0, 0));
	second.setCollisionBoxType(CollisionBoxType::cube);

	std::vector<std::shared_ptr<ObjectCore>> world;
	world.push_back(std::make_shared<ObjectCore>(first));
	world.push_back(std::make_shared<ObjectCore>(second));

	ObjectCore& subject = *world[0];
	CollisionResult result = subject.checkCollision(world);

	REQUIRE(result.collided);
	REQUIRE(!result.collidedObjectList.empty());
	REQUIRE(result.collidedObjectList[0].object.get() == world[1].get());
	REQUIRE(subject.getPosition().x != 0.0f);
}

TEST_CASE("ObjectCore collision reaction damages objects that collide fast enough", "[ObjectCore]")
{
	ObjectCore object(glm::vec3(2, 2, 2));
	object.setCollisionBoxType(CollisionBoxType::cube);
	object.setMovement(glm::vec3(10, 0, 0));

	CollisionResult result;
	result.collided = true;
	result.movementBeforeCollision = glm::vec3(10, 0, 0);

	object.reactToCollision(result, 1000.0f);

	REQUIRE(object.getHealth() < 100);
}

TEST_CASE("ObjectCore fall adds gravity when not standing on another object", "[ObjectCore]")
{
	ObjectCore object(glm::vec3(2, 2, 2));
	object.setPosition(glm::vec3(0, 10, 0));

	object.fall(100.0f, glm::vec3(0, -9.8f, 0));

	REQUIRE(object.getMovement().y < 0.0f);
}

TEST_CASE("ObjectCore does not fall when standing on something", "[ObjectCore]")
{
	ObjectCore object(glm::vec3(2, 2, 2));
	object.setPosition(glm::vec3(0, 10, 0));

	CollisionResult result;
	result.collided = true;
	result.movementBeforeCollision = glm::vec3(0, 0, 0);

	CollidedObject collidedObject;
	collidedObject.onTop = true;
	result.collidedObjectList.push_back(collidedObject);

	object.setOnOtherObject(true);
	object.fall(100.0f, glm::vec3(0, -9.8f, 0));

	REQUIRE(object.getMovement() == glm::vec3(0, 0, 0));
}