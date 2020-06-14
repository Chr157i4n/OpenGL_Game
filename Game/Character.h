#pragma once
#include "FPSCamera.h"
#include "defines.h"
#include "Model.h"
#include "Object.h"

class Character : public Object
{
public:

	Character(Shader* shader);

	void moveForward(std::vector<Object*> objects);

	void moveBackward(std::vector<Object*> objects);

	void moveRight(std::vector<Object*> objects);

	void moveLeft(std::vector<Object*> objects);

	void jump();

	//deltaTime in seconds
	void fall(float32 deltaTime);

	void crouch(bool crouch = true);

	void run(bool run = true);

protected:

	bool isCrouched = false;
	bool isRunning = false;

	glm::vec3 lookDirection;
	glm::vec3 speed;

	float32 forwardSpeed = 0.2;
	float32 backwardSidewaySpeed = 0.1;
	float32 upwardSpeed = 0;

	const float32 heigth = 4;

	glm::vec3 up;
};

