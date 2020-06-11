#pragma once
#include "Object.h"

class Bullet : public Object
{
public:
	Bullet(Shader* shader, glm::vec3 position, glm::vec3 rotation, glm::vec3 direction);

	void move(float32 deltaTime);


private:

};

