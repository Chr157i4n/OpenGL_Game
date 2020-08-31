#pragma once
#include "Object.h"

class Button : public Object
{
public:
	Button(Shader* shader, std::string modelFileName);

	virtual void interact_click();

	virtual void interact_hold();

	void setInteractLuaFunction_click(std::string newInteractLuaFunction)
	{
		interactLuaFunction_click = newInteractLuaFunction;
	}

	std::string getInteractLuaFunction_click()
	{
		return interactLuaFunction_click;
	}

	void setInteractLuaFunction_hold(std::string newInteractLuaFunction)
	{
		interactLuaFunction_hold = newInteractLuaFunction;
	}

	std::string getInteractLuaFunction_hold()
	{
		return interactLuaFunction_hold;
	}

	virtual void move();

private:
	std::string interactLuaFunction_click = "";
	std::string interactLuaFunction_hold = "";

	glm::vec3 originalPosition;
public:
	glm::vec3 travel = glm::vec3(0, 0, 0);
	float travel_duration = 0;

	std::chrono::system_clock::time_point lastInteractTimestamp = std::chrono::system_clock::now() - std::chrono::hours(1);
};

