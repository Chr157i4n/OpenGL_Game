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

	createHealthbar();
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

void Player::registerHit()
{
	addToHealth(-20);
	healthBar->setValue(health);
}

void Player::reactToCollision(CollisionResult collisionResult)
{
	float speed = glm::length(collisionResult.movementBeforeCollision);
	if (speed > 1)
	{
		addToHealth(-10 * speed);
		
	}
}

void Player::createHealthbar()
{
	healthBar = new UI_Element_ProgressBar(10, 10, 100, 20, 0, 0, false);
	healthBar->setColor(glm::vec4(1, 0, 0, 0.5));
	healthBar->setOutlineColor(glm::vec4(0.2, 0.2, 0.2, 0.5));
	healthBar->setValue(100);

	UI::addElement(healthBar);
}

void Player::addToHealth(float32 addHealth)
{
	health += addHealth;
	healthBar->setValue(health);

	if (addHealth < -10)
	{
		Renderer::applyPostprocessingEffect(PostProcessingEffect::blood, 0.005 * std::abs(health));
	}

	if (addHealth > 0)
		Logger::log(printObject() + " got healed by " + std::to_string((int)addHealth) + ". New Health: " + std::to_string((int)health));
	if (addHealth < 0)
		Logger::log(printObject() + " got " + std::to_string((int)addHealth) + " damage. New Health: " + std::to_string((int)health));

	if (health <= 0)
	{
		Logger::log(printObject() + " got destroyed");
		UI_Element* victoryLabel = new UI_Element_Label(UI::getWidth() / 2 - 80, UI::getHeight() / 2, "Du bist gestorben", 5, 1, glm::vec4(1, 0, 0, 1), false);
		UI::addElement(victoryLabel);
		Game::setGameState(GameState::GAME_GAME_OVER);
	}
}
