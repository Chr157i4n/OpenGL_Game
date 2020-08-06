#pragma once
#include "defines.h"

#include <string>

#include "UI_Element.h"

class UI_Element_Slider : public UI_Element
{
public:

	UI_Element_Slider(int x, int y, int w, int h, uint64 lifespan, std::string label, glm::vec4 foreColor = glm::vec4(1.0, 1.0, 1.0, 1.0), glm::vec4 backColor = glm::vec4(0.2, 0.2, 0.2, 0.4), bool isDebugInfo = false);

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

    void setValue(int value)
    {
        if(value>=0 && value<=100)
            this->value = value;
    }

    float getValue()
    {
        return value;
    }

    virtual void onMouseDrag(float mouseX, float mouseY, SDL_MouseButtonEvent* buttonEvent = nullptr);

    virtual void increase();

    virtual void decrease();

private:
    std::string label;
    int fontSize = 1;
    int labelOffsetX, labelOffsetY;
    float value=0;
};

