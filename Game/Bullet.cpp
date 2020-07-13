#include "Bullet.h"

Bullet::Bullet(Shader* shader, glm::vec3 position, glm::vec3 rotation, glm::vec3 direction) : Object(shader, "models/arrow.bmf")
{
	this->setType(ObjectType::Object_Bullet);
	this->position = position;
	this->rotation = rotation;
	this->movement = glm::normalize(direction)*speed;
	this->name = "Bullet";
}

void Bullet::fall(float32 deltaTime)
{
	if (position.y > 1)
	{
		movement.y -= 9.81 * 100 * deltaTime;

		float gegk = movement.y;
		float ank = sqrt(pow(movement.x, 2) + pow(movement.z, 2));

		rotation.z = atan(gegk / ank)* 180 / 3.14;
		rotation.z += 270;
	}

	if (position.y <= 1)
	{
		movement = glm::vec3(0, 0, 0);
		//position.y = 0;
	}
}

void Bullet::move(float32 deltaTime)
{
	position += movement;
}

void Bullet::checkHit(std::vector<Object*> objects)
{

		if (movement.x == 0 && movement.z == 0) return;

		CollisionResult collisionresult = checkCollision(objects);

		if(collisionresult.collided)
		{
			if (collisionresult.collidedWith->getType() == ObjectType::Object_Bullet) return;
			if (collisionresult.collidedWith->getType() == ObjectType::Object_Environment) return;
			if (collisionresult.collidedWith->getType() == ObjectType::Object_Player) return;
			if (collisionresult.collidedWith->getType() == ObjectType::Object_Entity) return;

			this->registerHit();
			collisionresult.collidedWith->registerHit();
			Logger::log("Hit");
		}

}
