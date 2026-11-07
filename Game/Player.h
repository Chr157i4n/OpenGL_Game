#pragma once
#include "defines.h"

#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Camera_FPS.h"
#include "UI_Element_ProgressBar.h"
#include "Character.h"

class Player : public Character, public FPSCamera
{

public:

	Player(Shader* shader, float fov, float width, float height);

	glm::vec3 getLookDirection();

	virtual glm::vec3 getLookOrigin();

	void onMouseMove(float xRel, float yRel);

	void updateCameraPosition();

	glm::vec3 getCameraPosition();

	void setCameraPosition(glm::vec3 newPosition);

	void activateFlashlight(bool enabled);

	void toggleFlashlight();

	void registerHit();

	virtual void reactToCollision(CollisionResult collisionResult);

	void createHealthbar();

	virtual void addToHealth(float32 addHealth);

	void spawn(glm::vec3 position = glm::vec3(20,0,0), glm::vec3 lookAt = glm::vec3(1,0,0));

	void move();

private:

	bool flashlightActive = false;
	UI_Element_ProgressBar* healthBar;

	glm::vec3 cameraOffset = glm::vec3(0, 2.8, 0);
	



};

