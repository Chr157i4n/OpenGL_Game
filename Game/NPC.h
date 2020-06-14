#pragma once
#include "Character.h"

class NPC : public Character
{

public:
	
	NPC(Shader* shader, float fov, float width, float height);

	void followCharacter(float32 deltaTime, std::vector<Object*> objects, Character* character);

};

