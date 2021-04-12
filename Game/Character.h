#pragma once
#include "defines.h"

#include <chrono>

#include "Object.h"

#include "Model.h"
#include "Bullet.h"


class Character : public Object
{
public:
	Character(Shader* shader);

	virtual glm::vec3 getLookDirection();

	virtual glm::vec3 getLookOrigin();

	virtual void interactWithObject();

	void resetVerticalMovement();

	void moveForward();

	void moveBackward();

	void moveRight();

	void moveLeft();

	void jump();

	void activateJumping();

	void crouch(bool crouch = true);

	void run(bool run = true);

	std::shared_ptr<Object> getObjectLookingAt()
	{
		return objectLookingAt;
	}

	std::shared_ptr<Object> calculateObjectLookingAt();

	glm::vec3 getRotationInv()
	{
		return rotation + glm::vec3(0, 180, 0);
	}

	virtual std::shared_ptr<Bullet> shoot();

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

	std::chrono::system_clock::time_point lastTimeShot = std::chrono::system_clock::now() - std::chrono::hours(1);

	glm::vec3 up;

	int team = 0;

	std::shared_ptr<Object> objectLookingAt;
};

