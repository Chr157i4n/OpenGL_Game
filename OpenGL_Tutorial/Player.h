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

	void moveForward(std::vector<Object*> objects);

	void moveBackward(std::vector<Object*> objects);

	void moveRight(std::vector<Object*> objects);

	void moveLeft(std::vector<Object*> objects);

	void onMouseMove(float xRel, float yRel);

	Bullet* shoot();

	void updateCameraPosition();

	glm::vec3 getCameraPosition();

	void setCameraPosition(glm::vec3 newPosition);


private:
	



};

