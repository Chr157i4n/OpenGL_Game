#pragma once
#include "FPSCamera.h"
#include "defines.h"
#include "Model.h"
#include "Object.h"
#include "Bullet.h"


class Character : public Object
{
public:
	Character(Shader* shader);

	virtual glm::vec3 getLookDirection();

	void resetVerticalMovement();

	void moveForward();

	void moveBackward();

	void moveRight();

	void moveLeft();

	void jump();

	void activateJumping();

	void crouch(bool crouch = true);

	void run(bool run = true);

	Object* getObjectLookingAt();

	std::shared_ptr<Bullet> shoot();

protected:

	bool isCrouched = false;
	bool isRunning = false;
	bool canJump = true;

	glm::vec3 lookDirection;
	glm::vec3 speed;

	float32 forwardSpeed = 0.2;
	float32 backwardSidewaySpeed = 0.1;
	float32 upwardSpeed = 0;

	const float32 heigth = 4;

	float32 lastTimeShot=0;

	glm::vec3 up;
};

