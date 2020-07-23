#pragma once
#include "Object.h"

class Bullet : public Object
{
public:
	Bullet(Shader* shader, glm::vec3 position, glm::vec3 rotation, glm::vec3 direction);

	void fall(float32 deltaTime);

	void move(float32 deltaTime);

	void checkHit(std::vector< std::shared_ptr<Object>> objects);

	virtual void registerHit();


private:

	float speed = 1.0;

};

