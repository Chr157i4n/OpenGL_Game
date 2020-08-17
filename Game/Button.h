#pragma once
#include "Object.h"

class Button : public Object
{
public:
	Button(Shader* shader, std::string modelFileName);

	virtual void interact();

private:

};

