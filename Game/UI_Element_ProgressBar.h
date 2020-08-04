#pragma once
#include "defines.h"

#include <GL/glew.h>

#include "UI_Element.h"

class UI_Element_ProgressBar : public UI_Element
{
public:
    UI_Element_ProgressBar(int x, int y, int w, int h, float value=0, uint64 lifespan=0, glm::vec4 foreColor = glm::vec4(1.0, 1.0, 1.0, 1.0), glm::vec4 backColor = glm::vec4(0.2, 0.2, 0.2, 0.4), bool isDebugInfo=false);

    virtual void drawUI_Element();

    float getValue() const;
    void setValue(float value);

private:
	float value = 0;
};

