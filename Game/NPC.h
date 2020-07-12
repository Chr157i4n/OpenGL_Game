#pragma once
#include "Character.h"

enum CurrentTask {
	Idle = 0,
	Follow_Character = 1,
	Follow_NavPoint = 2
};

class NPC : public Character
{

public:
	
	NPC(Shader* shader);

	void followCharacter(float32 deltaTime, std::vector<Object*> objects, Character* character);

	void followNavPoints(float32 deltaTime, std::vector<Object*> objects);

	void addNavPoint(glm::vec3 newNavPoint);

	void setNavPoints(std::vector<glm::vec3> newNavPoints);

	std::vector<glm::vec3> getNavPoints();

	void doCurrentTask(float32 deltaTime, std::vector<Object*> objects, std::vector<Character*> character);

	void setCurrentTask(CurrentTask newCurrentTask);

	CurrentTask getGurrentTask();

private:

	std::vector<glm::vec3> navPoints;
	int currentNavPoint = 0;
	CurrentTask currentTask = CurrentTask::Idle;
};

