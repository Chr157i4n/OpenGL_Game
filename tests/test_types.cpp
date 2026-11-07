#include <catch2/catch_test_macros.hpp>

#include "Types.h"

TEST_CASE("ObjectType uses a consistent bitmask", "[Types]")
{
    REQUIRE(ObjectType::Object_Player == 1);
    REQUIRE(ObjectType::Object_NPC == 2);
    REQUIRE(ObjectType::Object_Character == 4);
    REQUIRE(ObjectType::Object_Entity == 8);
    REQUIRE(ObjectType::Object_Interactable == 16);
    REQUIRE(ObjectType::Object_Environment == 32);
    REQUIRE(ObjectType::Object_Bullet == 64);
}

TEST_CASE("ObjectType flag combination via operator|", "[Types]")
{
    ObjectType combined = ObjectType::Object_Player | ObjectType::Object_Character;
    REQUIRE((combined & ObjectType::Object_Player) != 0);
    REQUIRE((combined & ObjectType::Object_Character) != 0);
    REQUIRE((combined & ObjectType::Object_Bullet) == 0);
}

TEST_CASE("objectTypeTable maps names to the enum", "[Types]")
{
    REQUIRE(objectTypeTable.at("Bot") == ObjectType::Object_NPC);
    REQUIRE(objectTypeTable.at("Bullet") == ObjectType::Object_Bullet);
    REQUIRE(objectTypeTable.at("Environment") == ObjectType::Object_Environment);
    REQUIRE(objectTypeTable.at("Player") == ObjectType::Object_Player);
    REQUIRE(objectTypeTable.at("Entity") == ObjectType::Object_Entity);
}

TEST_CASE("CollisionBoxTypeTable maps names to the enum", "[Types]")
{
    REQUIRE(CollisionBoxTypeTable.at("none") == CollisionBoxType::none);
    REQUIRE(CollisionBoxTypeTable.at("cube") == CollisionBoxType::cube);
    REQUIRE(CollisionBoxTypeTable.at("prism") == CollisionBoxType::prism);
}

TEST_CASE("CollisionResult default-initialises to no collision", "[Types]")
{
    CollisionResult result;
    REQUIRE(result.collided == false);
    REQUIRE(result.collidedObjectList.empty());
}
