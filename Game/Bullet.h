#pragma once
#include "defines.h"

#include "Object.h"

class Bullet : public Object
{
public:
	Bullet(Shader* shader, glm::vec3 position, glm::vec3 rotation, glm::vec3 direction);

	Bullet(Shader* shader);

	//void move();

	void fall();

	void checkHit();

	virtual void registerHit();


private:

	float speed = 70;

};

