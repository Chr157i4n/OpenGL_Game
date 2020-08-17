#include "Button.h"

#include "LuaManager.h"


Button::Button(Shader* shader, std::string modelFileName) : Object(shader, modelFileName)
{

}

void Button::interact()
{
	if (!this->getEnabled()) return;

	this->markObject();
	LuaManager::runFunction();
}
