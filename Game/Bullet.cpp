#include "Bullet.h"

Bullet::Bullet(Shader* shader, glm::vec3 position, glm::vec3 rotation, glm::vec3 direction) : Object(shader, "models/arrow.bmf")
{
	this->setType(ObjectType::Object_Bullet);
	this->position = position;
	this->rotation = rotation;
}

void Bullet::move(float32 deltaTime)
{
	if (position.y > 1)
	{
		//speedY -= 9.81 * 50 * deltaTime;
	}

	if (position.y <= 1)
	{
		//speedXZ = 0;
		//speedY = 0;
	}
}
