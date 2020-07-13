#include "Player.h"

Player::Player(Shader* shader, float fov, float width, float height) : Character(shader), FPSCamera(fov, width, height)
{
	setType(ObjectType::Object_Player | ObjectType::Object_Character);
	

	position.x = 20;
	position.y = 0;
	position.z = 0;

	cameraposition.x = position.x;
	cameraposition.y = position.y + 4;
	cameraposition.z = position.z;

	this->name = "Player";

}

glm::vec3 Player::getLookDirection()
{
	return lookAt;
}

void Player::onMouseMove(float xRel, float yRel)
{
	onMouseMoved(xRel, yRel);
	setRotation(glm::vec3(0, -yaw + 90, 0));
}

Bullet* Player::shoot()
{
	Bullet* newBullet = new Bullet(shader, cameraposition, glm::vec3(0, -yaw, pitch - 90), lookAt);
	//newBullet->setType(ObjectType::Object_Bullet);
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



