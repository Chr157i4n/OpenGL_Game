#pragma once
#include "Character.h"
#include "Bullet.h"
#include "libs/glm/glm.hpp"
#include "libs/glm/ext/matrix_transform.hpp"
#include "libs/glm/gtc/matrix_transform.hpp"

class Player : public Character, public FPSCamera
{

public:

	Player(Shader* shader, float fov, float width, float height);

	glm::vec3 getLookDirection();

	void onMouseMove(float xRel, float yRel);

	std::shared_ptr<Bullet> shoot();

	void updateCameraPosition();

	glm::vec3 getCameraPosition();

	void setCameraPosition(glm::vec3 newPosition);

	void activateFlashlight(bool enabled);

	void toggleFlashlight();

private:

	bool flashlightActive = false;
	



};

