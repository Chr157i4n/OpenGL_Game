#include "Raypicker.h"

#include "Game.h"

RayPickingResult Raypicker::doRayPicking(Ray ray, std::shared_ptr<Object> object)
{
	RayPickingResult result;
	
	if (!object) return result;
	if (object->getType() & ObjectType::Object_Player) return result;
	if (object->getType() & ObjectType::Object_Environment) return result;
	if (!object->getEnabled()) return result;
	
	

	float minX = object->getOrientedBoundingBox().minX;
	float maxX = object->getOrientedBoundingBox().maxX;

	float minY = object->getOrientedBoundingBox().minY;
	float maxY = object->getOrientedBoundingBox().maxY;

	float minZ = object->getOrientedBoundingBox().minZ;
	float maxZ = object->getOrientedBoundingBox().maxZ;

	/*minX = object->getPosition().x - object->getDimensions().x / 2;
	maxX = object->getPosition().x + object->getDimensions().x / 2;

	minY = object->getPosition().y;
	maxY = object->getPosition().y + object->getDimensions().y;

	minZ = object->getPosition().z - object->getDimensions().z / 2;
	maxZ = object->getPosition().z + object->getDimensions().z / 2;*/



	float tmin = (minX - ray.origin.x) / ray.direction.x;
	float tmax = (maxX - ray.origin.x) / ray.direction.x;

	if (tmin > tmax) std::swap(tmin, tmax);

	float tymin = (minY - ray.origin.y) / ray.direction.y;
	float tymax = (maxY - ray.origin.y) / ray.direction.y;

	if (tymin > tymax) std::swap(tymin, tymax);

	if ((tmin > tymax) || (tymin > tmax))
		return result;

	if (tymin > tmin)
		tmin = tymin;

	if (tymax < tmax)
		tmax = tymax;

	float tzmin = (minZ - ray.origin.z) / ray.direction.z;
	float tzmax = (maxZ - ray.origin.z) / ray.direction.z;

	if (tzmin > tzmax) std::swap(tzmin, tzmax);

	if ((tmin > tzmax) || (tzmin > tmax))
		return result;

	if (tzmin > tmin)
		tmin = tzmin;

	if (tzmax < tmax)
		tmax = tzmax;

	//Ray is intersecting with plane
	//test for right direction

	if (glm::dot(ray.direction, object->getPosition() - ray.origin) < 0)
	{
		//Logger::log("facing wrong direction");
		result.intersect = false;
	}
	else
	{
		//Logger::log("facing right direction");
		result.intersect = true;
		IntersectedObject intersectedObject;
		intersectedObject.object = object;

		//t0x = (B0x - Ox) / Dx
		//t0y = (B0y - Oy) / Dy
		//t0z = (B0z - Oz) / Dz

		result.intersectedObject_nearest = intersectedObject;
	}

	return result;
}


RayPickingResult Raypicker::doRayPicking(Ray ray, std::vector<std::shared_ptr<Object>> objects)
{
	RayPickingResult result, subresult;

#ifdef DEBUG_RAYPICKING
	Logger::log("Starting Raypicking");
#endif
	for (std::shared_ptr<Object> object : objects)
	{
		subresult = Raypicker::doRayPicking(ray, object);
		if (subresult.intersect)
		{
			result.intersectedObjects.push_back(subresult.intersectedObject_nearest);
		}
	}

#ifdef DEBUG_RAYPICKING
	Logger::log("Determine nearest Object");
#endif
	result.intersectedObject_nearest = Raypicker::getNearestObject(ray.origin, result.intersectedObjects);

#ifdef DEBUG_RAYPICKING
	Logger::log("Finished Raypicking");
#endif
	return result;
}


IntersectedObject Raypicker::getNearestObject(glm::vec3 origin, std::vector<IntersectedObject> objects)
{
	float minDistance = (std::numeric_limits<float>::max)();
	float currentDistance = 0;
	IntersectedObject objectMinDistance;

	for (IntersectedObject object : objects)
	{
		currentDistance = object.object->getDistance(origin);
		if (currentDistance < minDistance)
		{
			minDistance = currentDistance;
			objectMinDistance = object;
		}
	}	
	return objectMinDistance;
}
