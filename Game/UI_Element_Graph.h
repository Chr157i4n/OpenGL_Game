#pragma once
#include "defines.h"

#include <GL/glew.h>
#include <vector>

#include "UI_Element.h"

class UI_Element_Graph : public UI_Element
{
public:
    UI_Element_Graph(int x, int y, int w, int h, uint64 lifespan, glm::vec4 color = glm::vec4(1, 1, 1, 1), bool isDebugInfo = false);

    virtual void drawUI_Element();

private:
    std::vector<float32> valueBuffer;
    glm::vec4 axisColor = glm::vec4(1, 1, 1, 1);

public:
    float32 getValue() const;
    void setValue(float value);

    glm::vec4 getAxisColor() const;
    void setAxisColor(glm::vec4 axisColor);
};

