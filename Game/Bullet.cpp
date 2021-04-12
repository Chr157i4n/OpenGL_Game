#include "Bullet.h"
#include "Game.h"

Bullet::Bullet(Shader* shader, glm::vec3 position, glm::vec3 rotation, glm::vec3 direction) : Object(shader, "arrow.bmf")
{
	this->setType(ObjectType::Object_Bullet);
	this->position = position;
	this->rotation = rotation;
	this->movement = glm::normalize(direction) * speed;// *Game::getDelta(); //todo : apply Game::getDelta() per frame
	this->name = "Bullet";
	this->setCollisionBoxType(CollisionBoxType::cube);
}

Bullet::Bullet(Shader* shader) : Object(shader, "arrow.bmf")
{
	this->setType(ObjectType::Object_Bullet);
	this->name = "Bullet";
	this->setCollisionBoxType(CollisionBoxType::cube);
}

void Bullet::fall()
{
	this->Object::fall();

	if (movement == glm::vec3(0, 0, 0)) return;
	if (position.y < 0.5) { movement=glm::vec3(0, 0, 0); return; }


	float gegk = movement.y;
	float ank = sqrt(pow(movement.x, 2) + pow(movement.z, 2));

	rotation.z = atan(gegk / ank) * 180 / 3.14;
	rotation.z += 270;

}

void Bullet::checkHit()
{

	if (movement.x == 0 && movement.z == 0) return;

	CollisionResult collisionresult = checkCollision(); //doubled with the normal collision detection. this can be improved

	if (!collisionresult.collided) return;


	for (CollidedObject* collidedObject : collisionresult.collidedObjectList)
	{
		if (collidedObject->object->getType() & ObjectType::Object_Bullet) return;
		if (collidedObject->object->getType() & ObjectType::Object_Environment) return;
		//if (collidedObject->object->getType() & ObjectType::Object_Player) return;
		if (collidedObject->object->getType() & ObjectType::Object_Entity) return;

		this->registerHit();
		collidedObject->object->registerHit();
		Logger::log(collidedObject->object->printObject() + " was hit");
		
		AudioManager::play3D("audio/hit.wav", this->getPosition());
	}


}

void Bullet::registerHit()
{
	this->health = 0;
}
