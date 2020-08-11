#pragma once
#include "defines.h"

#include <string>
#include <vector>

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

    virtual int onMouseClick(float mouseX, float mouseY, SDL_MouseButtonEvent* buttonEvent = nullptr);

    void addState(std::string stateLabel)
    {
        states.push_back(stateLabel);
    }

    int nextState()
    {
        state = (state + 1) % states.size();
        this->setLabel(states[state]);

        return state;
    }

    void setState(int state)
    {
        if (state >= 0 && state < states.size())
        {
            this->state = state;
            this->setLabel(states[state]);
        }
    }



private:
    std::string label;

    int state=0;
    std::vector<std::string> states;
    int fontSize = 1;
    int labelOffsetX, labelOffsetY;
};

