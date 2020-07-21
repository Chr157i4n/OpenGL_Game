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

	void followCharacter(float32 deltaTime, std::vector< std::shared_ptr<Object>> objects, std::shared_ptr<Character> character);

	void followNavPoints(float32 deltaTime, std::vector< std::shared_ptr<Object>> objects);

	void addNavPoint(glm::vec3 newNavPoint);

	void setNavPoints(std::vector<glm::vec3> newNavPoints);

	std::vector<glm::vec3> getNavPoints();

	void doCurrentTask(float32 deltaTime, std::vector< std::shared_ptr<Object>> objects, std::vector< std::shared_ptr<Character>> character);

	void setCurrentTask(CurrentTask newCurrentTask);

	CurrentTask getGurrentTask();

	void evade(float32 deltaTime, CollisionResult collisionResult, std::vector< std::shared_ptr<Object>> objects);

private:

	std::vector<glm::vec3> navPoints;
	int currentNavPoint = 0;
	CurrentTask currentTask = CurrentTask::Idle;
};

