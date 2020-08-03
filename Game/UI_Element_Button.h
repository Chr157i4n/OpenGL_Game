#pragma once
#include "defines.h"

#include <string>

#include "UI_Element.h"


class UI_Element_Button : public UI_Element
{
public:
    UI_Element_Button(int x, int y, int w, int h, uint64 lifespan, std::string label, glm::vec4 color = glm::vec4(1, 1, 1, 1), bool isDebugInfo = false);

    virtual void drawUI_Element();

    void setLabel(std::string newLabel)
    {
        label = newLabel;
    }

    std::string getLabel()
    {
        return label;
    }

    void setFontSize(int fontSize)
    {
        this->fontSize = fontSize;
    }

    int getFontSize()
    {
        return fontSize;
    }



private:
    std::string label;
    glm::vec4 backColor;
    int fontSize = 1;
};

