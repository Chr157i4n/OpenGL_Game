#pragma once
#include "Character.h"

class NPC : public Character
{

public:
	
	NPC(Shader* shader);

	void followCharacter(float32 deltaTime, std::vector<Object*> objects, Character* character);

	void followNavPoints(float32 deltaTime, std::vector<Object*> objects);

	void addNavPoint(glm::vec3 newNavPoint);

	void setNavPoints(std::vector<glm::vec3> newNavPoints);

	std::vector<glm::vec3> getNavPoints();

private:

	std::vector<glm::vec3> navPoints;
	int currentNavPoint = 0;

};

