#pragma once
#include "defines.h"

#include <GL/glew.h>
#include <vector>

#include "UI_Element.h"

class UI_Element_Graph : public UI_Element
{
public:
    UI_Element_Graph(int x, int y, int w, int h, uint64 lifespan, glm::vec4 foreColor = glm::vec4(1.0, 1.0, 1.0, 1.0), glm::vec4 backColor = glm::vec4(0.2, 0.2, 0.2, 0.4), bool isDebugInfo = false);

    virtual void drawUI_Element();

private:
    std::vector<float32> valueBuffer;

public:
    float32 getValue() const;
    void setValue(float value);
};

