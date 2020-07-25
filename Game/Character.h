#pragma once
#include "defines.h"

#include "Object.h"

#include "Model.h"
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

	int getTeam() {
		return team;
	}

	void setTeam(int newTeam){
		team = newTeam;
	}

protected:

	bool isCrouched = false;
	bool isRunning = false;
	bool canJump = true;

	glm::vec3 lookDirection;


	float32 forwardSpeed = 10;				//per second
	float32 backwardSidewaySpeed = 5;		//per second
	float32 upwardSpeed = 0;				//per second

	const float32 heigth = 4;

	float32 lastTimeShot=0;

	glm::vec3 up;

	int team = 0;
};

