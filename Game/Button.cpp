#include "Button.h"

#include "LuaManager.h"


Button::Button(Shader* shader, std::string modelFileName) : Object(shader, modelFileName)
{
	this->setType(ObjectType::Object_Interactable | ObjectType::Object_Entity);
}

void Button::interact()
{
	if (!this->getEnabled()) return;

	this->markObject();
	LuaManager::runFunction(interactLuaFunction);
}
