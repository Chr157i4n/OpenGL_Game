#include "Bullet.h"
#include "Game.h"

Bullet::Bullet(Shader* shader, glm::vec3 position, glm::vec3 rotation, glm::vec3 direction) : Object(shader, "arrow.bmf")
{
	this->setType(ObjectType::Object_Bullet);
	this->position = position;
	this->rotation = rotation;
	this->movement = glm::normalize(direction) * speed;
	this->name = "Bullet";
	this->setCollisionBoxType(CollisionBoxType::cube);
}

void Bullet::fall(float32 deltaTime)
{
	if (movement == glm::vec3(0, 0, 0)) return;
	if (position.y < 0.5) { movement=glm::vec3(0, 0, 0); return; }


	movement.y -= 9.81 * 100 * deltaTime;

	float gegk = movement.y;
	float ank = sqrt(pow(movement.x, 2) + pow(movement.z, 2));

	rotation.z = atan(gegk / ank) * 180 / 3.14;
	rotation.z += 270;

}

void Bullet::move(float32 deltaTime)
{
	position += movement;
}

void Bullet::checkHit(std::vector< std::shared_ptr<Object>> objects)
{

	if (movement.x == 0 && movement.z == 0) return;

	CollisionResult collisionresult = checkCollision(objects); //doubled with the normal collision detection. this can be improved

	if (!collisionresult.collided) return;


	for (CollidedObject* collidedObject : collisionresult.collidedObjectList)
	{
		if (collidedObject->object->getType() == ObjectType::Object_Bullet) return;
		if (collidedObject->object->getType() == ObjectType::Object_Environment) return;
		if (collidedObject->object->getType() == ObjectType::Object_Player) return;
		if (collidedObject->object->getType() == ObjectType::Object_Entity) return;

		this->registerHit();
		collidedObject->object->registerHit();
		Logger::log(collidedObject->object->printObject() + " was hit");
		Game::getSoundEngine()->play2D("audio/hit.wav", false);
	}


}
