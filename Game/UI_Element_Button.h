#pragma once
#include "defines.h"

#include <string>

#include "UI_Element.h"


class UI_Element_Button : public UI_Element
{
public:
    UI_Element_Button(int x, int y, int w, int h, uint64 lifespan, std::string label, glm::vec4 foreColor = glm::vec4(1, 1, 1, 1), glm::vec4 backColor = glm::vec4(0.2, 0.2, 0.2, 0.4), bool isDebugInfo = false);

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

    virtual void onMouseClick(float mouseX, float mouseY, SDL_MouseButtonEvent* buttonEvent = nullptr);



private:
    std::string label;
    int fontSize = 1;
    int labelOffsetX, labelOffsetY;
};

