#include "Player.h"
#include "Game.h"

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

std::shared_ptr<Bullet> Player::shoot()
{
	std::shared_ptr<Bullet> newBullet = std::make_shared<Bullet>(shader, cameraposition, glm::vec3(0, -yaw, pitch - 90), lookAt);
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
}

void Player::activateFlashlight(bool enabled)
{
	glm::vec3 spotLightColor = glm::vec3(0);

	if (enabled)
	{
		spotLightColor = glm::vec3(1.0f);
	}
	else {
		spotLightColor = glm::vec3(0);
	}
	shader->bind();
	GLCALL(glUniform3fv(glGetUniformLocation(shader->getShaderId(), "u_spot_light.diffuse"), 1, (float*)&spotLightColor));
	GLCALL(glUniform3fv(glGetUniformLocation(shader->getShaderId(), "u_spot_light.specular"), 1, (float*)&spotLightColor));
	spotLightColor *= 0.2f;
	GLCALL(glUniform3fv(glGetUniformLocation(shader->getShaderId(), "u_spot_light.ambient"), 1, (float*)&spotLightColor));
	shader->unbind();
}

void Player::toggleFlashlight()
{
	glm::vec3 spotLightColor = glm::vec3(0);
	flashlightActive = !flashlightActive;

	if (flashlightActive)
	{
		spotLightColor = glm::vec3(1.0f);
		shader->bind();
		GLCALL(glUniform3fv(glGetUniformLocation(shader->getShaderId(), "u_spot_light.diffuse"), 1, (float*)&spotLightColor));
		GLCALL(glUniform3fv(glGetUniformLocation(shader->getShaderId(), "u_spot_light.specular"), 1, (float*)&spotLightColor));
		spotLightColor *= 0.2f;
		GLCALL(glUniform3fv(glGetUniformLocation(shader->getShaderId(), "u_spot_light.ambient"), 1, (float*)&spotLightColor));
		shader->unbind();
	}
	else 
	{
		shader->bind();
		GLCALL(glUniform3fv(glGetUniformLocation(shader->getShaderId(), "u_spot_light.diffuse"), 1, (float*)&spotLightColor));
		GLCALL(glUniform3fv(glGetUniformLocation(shader->getShaderId(), "u_spot_light.specular"), 1, (float*)&spotLightColor));
		GLCALL(glUniform3fv(glGetUniformLocation(shader->getShaderId(), "u_spot_light.ambient"), 1, (float*)&spotLightColor));
		shader->unbind();
	}


}


