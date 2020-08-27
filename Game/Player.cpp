#include "Player.h"

#include "Game.h"
#include "Bullet.h"
#include "Shader.h"

Player::Player(Shader* shader, float fov, float width, float height) : Character(shader), FPSCamera(fov, width, height)
{
	setType(ObjectType::Object_Player | ObjectType::Object_Character);
	

	position.x = 20;
	position.y = 0;
	position.z = 0;

	cameraposition = position + cameraOffset;

	this->name = "Player";

	createHealthbar();
}

glm::vec3 Player::getLookDirection()
{
	return lookAt;
}

glm::vec3 Player::getLookOrigin()
{
	return this->getCameraPosition();
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
		cameraposition.y = position.y + cameraOffset.y * 0.6;
	}
	else {
		cameraposition.y = position.y + cameraOffset.y * 1;
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
		GLCALL(glUniform3fv(glGetUniformLocation(shader->getShaderId(), "u_spot_light.color"), 1, (float*)&spotLightColor));
		shader->unbind();
	}
	else 
	{
		shader->bind();
		GLCALL(glUniform3fv(glGetUniformLocation(shader->getShaderId(), "u_spot_light.color"), 1, (float*)&spotLightColor));
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
	this->Object::reactToCollision(collisionResult);
}

void Player::createHealthbar()
{
	healthBar = new UI_Element_ProgressBar(10, 10, 100, 20, 0, 0);
	healthBar->setForeColor(glm::vec4(1, 0, 0, 0.5));
	healthBar->setBackColor(glm::vec4(0.2, 0.2, 0.2, 0.4));
	healthBar->setValue(100);

	UI::addElement(healthBar);
}

void Player::addToHealth(float32 addHealth)
{
	health += addHealth;
	healthBar->setValue(health);

	if (addHealth < -10)
	{
		Renderer::applyPostprocessingEffect(PostProcessingEffect::blood, 0.005 * std::abs(addHealth));
	}

	if (addHealth > 0)
		Logger::log(printObject() + " got healed by " + std::to_string((int)addHealth) + ". New Health: " + std::to_string((int)health));
	if (addHealth < 0)
		Logger::log(printObject() + " got " + std::to_string((int)addHealth) + " damage. New Health: " + std::to_string((int)health));

	if (health <= 0)
	{
		Logger::log(printObject() + " got destroyed");
		UI_Element* victoryLabel = new UI_Element_Label(Game::getWindowWidth() / 2 - 80, Game::getWindowHeight() / 2, 10, 10, "Du bist gestorben", 5000, 1, glm::vec4(1, 0, 0, 1), glm::vec4(0.2, 0.2, 0.2, 0.4), false);
		UI::addElement(victoryLabel);
		Game::setGameState(GameState::GAME_GAME_OVER);
	}
}

void Player::spawn(glm::vec3 position, glm::vec3 lookAt)
{
	this->setPosition(position);
	this->lookAt = lookAt;

	this->health = 100;

	this->setEnabled(true);
}

void Player::move()
{
	this->Object::move();

	if (movement != glm::vec3(0, 0, 0))
	{
		NetworkManager::sendPlayerPosition();
	}
}