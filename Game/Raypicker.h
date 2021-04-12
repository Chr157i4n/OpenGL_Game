#pragma once

#include <memory>
#include <vector>

#include "libs/glm/glm.hpp"

#include "Object.h"


struct Ray {
	Ray(glm::vec3 origin, glm::vec3 direction)
	{
		this->origin = origin;
		this->direction = direction;
	}

	glm::vec3 origin;
	glm::vec3 direction;
};

struct IntersectedObject {
	glm::vec3 intersectPosition = glm::vec3(0);
	std::shared_ptr<Object> object = nullptr;
};

struct RayPickingResult {
	bool intersect = false;

	std::vector<IntersectedObject> intersectedObjects;
	IntersectedObject intersectedObject_nearest;
};

class Raypicker
{
public:

	static RayPickingResult doRayPicking(Ray ray, std::shared_ptr<Object> object);

	static RayPickingResult doRayPicking(Ray ray, std::vector<std::shared_ptr<Object>> objects);

private:

	static IntersectedObject getNearestObject(glm::vec3 origin, std::vector<IntersectedObject> objects);
};

