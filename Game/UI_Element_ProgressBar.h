#pragma once
#include "defines.h"

#include <GL/glew.h>

#include "UI_Element.h"

class UI_Element_ProgressBar : public UI_Element
{
public:
    UI_Element_ProgressBar(int x, int y, int w, int h, float value=0, uint64 lifespan=0, bool isDebugInfo=false);

    virtual void drawUI_Element();

private:
	float value = 0;
    glm::vec4 outlineColor = glm::vec4(1, 1, 1, 1);

public:
    float getValue() const;
    void setValue(float value);

    glm::vec4 getOutlineColor() const;
    void setOutlineColor(glm::vec4 outlineColor);

};

