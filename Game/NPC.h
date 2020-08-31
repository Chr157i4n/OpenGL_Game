#pragma once
#include "defines.h"

#include <string>
#include <vector>

#include "Character.h"

#include <irrKlang.h>

enum CurrentTask {
	Idle = 0,
	Follow_Character = 1,
	Follow_NavPoint = 2,
	Attack = 3,
};

class NPC : public Character
{

public:

	NPC(Shader* shader);

	void followCharacter(std::shared_ptr<Character> character);

	void followNavPoints();

	void addNavPoint(glm::vec3 newNavPoint);

	void setNavPoints(std::vector<glm::vec3> newNavPoints);

	std::vector<glm::vec3> getNavPoints();

	void doCurrentTask();

	void setCurrentTask(CurrentTask newCurrentTask);

	CurrentTask getGurrentTask();

	void evade(CollisionResult collisionResult);

	void reactToCollision(CollisionResult collisionResult);

	void calculateNextTarget();

	void attackCurrentTarget();

	virtual void interact_click();

	virtual void move();

	virtual void registerHit();

	virtual void speak(std::string audioFile);


private:

	std::vector<glm::vec3> navPoints;
	int currentNavPoint = 0;
	CurrentTask currentTask = CurrentTask::Idle;

	std::shared_ptr<Character> currentTarget = nullptr;

	irrklang::ISound* voice = nullptr;
	std::vector<std::string> voicefiles;


};

