#include "Geometry.h"

#include <limits>

glm::vec3 calculateMeshBoundingBox(const std::vector<glm::vec3>& vertices)
{
	if (vertices.empty())
	{
		return glm::vec3(0, 0, 0);
	}

	glm::vec3 min = glm::vec3(std::numeric_limits<float>::infinity(),
		std::numeric_limits<float>::infinity(),
		std::numeric_limits<float>::infinity());

	glm::vec3 max = glm::vec3(-std::numeric_limits<float>::infinity(),
		-std::numeric_limits<float>::infinity(),
		-std::numeric_limits<float>::infinity());

	for (const glm::vec3& vertex : vertices)
	{
		min.x = (std::min)(min.x, vertex.x);
		min.y = (std::min)(min.y, vertex.y);
		min.z = (std::min)(min.z, vertex.z);

		max.x = (std::max)(max.x, vertex.x);
		max.y = (std::max)(max.y, vertex.y);
		max.z = (std::max)(max.z, vertex.z);
	}

	return max - min;
}

glm::vec3 calculateModelBoundingBox(const std::vector<glm::vec3>& vertices)
{
	glm::vec3 dimensions = calculateMeshBoundingBox(vertices);

	return glm::vec3(dimensions.x + dimensions.z,
		dimensions.y * 2,
		dimensions.x + dimensions.z);
}