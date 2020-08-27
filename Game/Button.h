#pragma once
#include "Object.h"

class Button : public Object
{
public:
	Button(Shader* shader, std::string modelFileName);

	virtual void interact();

	void setInteractLuaFunction(std::string newInteractLuaFunction)
	{
		interactLuaFunction = newInteractLuaFunction;
	}

	std::string getInteractLuaFunction()
	{
		return interactLuaFunction;
	}

private:
	std::string interactLuaFunction = "interact";

};

