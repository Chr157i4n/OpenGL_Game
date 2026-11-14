#include <catch2/catch_test_macros.hpp>

#include <vector>

#include "Geometry.h"

TEST_CASE("calculateMeshBoundingBox handles meshes that do not straddle the origin", "[Geometry]")
{
	std::vector<glm::vec3> vertices = {
		glm::vec3(10, 20, 30),
		glm::vec3(14, 25, 32)
	};

	glm::vec3 bounds = calculateMeshBoundingBox(vertices);

	REQUIRE(bounds.x == 4.0f);
	REQUIRE(bounds.y == 5.0f);
	REQUIRE(bounds.z == 2.0f);
}

TEST_CASE("calculateMeshBoundingBox returns zero for empty meshes", "[Geometry]")
{
	REQUIRE(calculateMeshBoundingBox({}) == glm::vec3(0, 0, 0));
}

TEST_CASE("calculateModelBoundingBox uses the conservative AABB rule", "[Geometry]")
{
	std::vector<glm::vec3> vertices = {
		glm::vec3(0, 0, 0),
		glm::vec3(2, 3, 4)
	};

	glm::vec3 bounds = calculateModelBoundingBox(vertices);

	REQUIRE(bounds == glm::vec3(6, 6, 6));
}