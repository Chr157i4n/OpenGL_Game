#include "Player.h"

Player::Player(Shader* shader, float fov, float width, float height) : Character(shader), FPSCamera(fov, width, height)
{
	setObjectType(ObjectType::Object_Player);
	

	position.x = 20;
	position.y = 0;
	position.z = 0;

	cameraposition.x = position.x;
	cameraposition.y = position.y + 4;
	cameraposition.z = position.z;

}

void Player::moveForward(std::vector<Object*> objects) {

	glm::vec3 v = glm::normalize(glm::vec3(1.0f, 0.0f, 1.0f) * lookAt) * forwardSpeed;

	for (Object* object : objects)
	{
		if (object->getObjectType() == ObjectType::Object_Player) continue;
		if (object->getObjectType() == ObjectType::Object_Bot) continue;
		if (object->getObjectType() == ObjectType::Object_Environment) continue;
		if (object->getObjectType() == ObjectType::Object_Bullet) continue;
		if (checkCollisionXY(object, position+v)) return;
	}
	if (checkBoundaries(objects[0], position+v)) return;

	position += v;

}

void Player::moveBackward(std::vector<Object*> objects) {

	glm::vec3 v = glm::normalize(glm::vec3(1.0f, 0.0f, 1.0f) * lookAt) * -backwardSidewaySpeed;

	for (Object* object : objects)
	{
		if (object->getObjectType() == ObjectType::Object_Player) continue;
		if (object->getObjectType() == ObjectType::Object_Bot) continue;
		if (object->getObjectType() == ObjectType::Object_Environment) continue;
		if (object->getObjectType() == ObjectType::Object_Bullet) continue;
		if (checkCollisionXY(object, position+v)) return;
	}
	if (checkBoundaries(objects[0], position+v)) return;

	position += v;

}

void Player::moveRight(std::vector<Object*> objects) {
	
	glm::vec3 v = glm::normalize(glm::vec3(1.0f, 0.0f, 1.0f) * glm::cross(lookAt, up)) * backwardSidewaySpeed;

	for (Object* object : objects)
	{
		if (object->getObjectType() == ObjectType::Object_Player) continue;
		if (object->getObjectType() == ObjectType::Object_Bot) continue;
		if (object->getObjectType() == ObjectType::Object_Environment) continue;
		if (object->getObjectType() == ObjectType::Object_Bullet) continue;
		if (checkCollisionXY(object, position+v)) return;
	}
	if (checkBoundaries(objects[0], position+v)) return;

	position += v;

}

void Player::moveLeft(std::vector<Object*> objects) {

	glm::vec3 v = glm::normalize(glm::vec3(1.0f, 0.0f, 1.0f) * glm::cross(lookAt, up)) * -backwardSidewaySpeed;

	for (Object* object : objects)
	{
		if (object->getObjectType() == ObjectType::Object_Player) continue;
		if (object->getObjectType() == ObjectType::Object_Bot) continue;
		if (object->getObjectType() == ObjectType::Object_Environment) continue;
		if (object->getObjectType() == ObjectType::Object_Bullet) continue;
		if (checkCollisionXY(object, position+v)) return;
	}
	if (checkBoundaries(objects[0], position+v)) return;

	position += v;

}

void Player::onMouseMove(float xRel, float yRel)
{
	onMouseMoved(xRel, yRel);
	setRotation(glm::vec3(0, -yaw + 90, 0));
}


Bullet* Player::shoot()
{
	Bullet* newBullet = new Bullet(shader, cameraposition, glm::vec3(0, -yaw, pitch - 90), lookAt);
	newBullet->setObjectType(ObjectType::Object_Bullet);
	return newBullet;
}

void Player::updateCameraPosition()
{
	cameraposition.x = position.x;
	cameraposition.z = position.z;

	if (isCrouched)
	{
		cameraposition.y = position.y + 2;
	}
	else {
		cameraposition.y = position.y + 4;
	}

	update();
}

glm::vec3 Player::getCameraPosition()
{
	return cameraposition;
}

void Player::setCameraPosition(glm::vec3 newPosition)
{
	cameraposition = newPosition;
	setPosition(newPosition);
}



