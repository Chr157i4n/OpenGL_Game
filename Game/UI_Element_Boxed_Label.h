#pragma once
#include "UI_Element_Label.h"

#include "defines.h"

#include <string>
#include "libs/glm/glm.hpp"

class UI_Element_Boxed_Label : public UI_Element_Label
{

public:
	UI_Element_Boxed_Label(int x, int y, int w, int h, std::string label, uint64 lifespan, int fontSize = 1, glm::vec4 foreColor = glm::vec4(1.0, 1.0, 1.0, 1.0), glm::vec4 backColor = glm::vec4(0.2, 0.2, 0.2, 0.4), bool isDebugInfo = false);

    virtual void drawUI_Element();

private:
    int labelOffsetX, labelOffsetY;

};

