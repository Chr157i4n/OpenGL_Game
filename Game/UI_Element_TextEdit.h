#pragma once
#include "UI_Element.h"

#include <regex>

class UI_Element_TextEdit : public UI_Element
{

public:
    UI_Element_TextEdit(int x, int y, int w, int h, std::string label, uint64 lifespan, int fontSize = 1, glm::vec4 foreColor = glm::vec4(1.0, 1.0, 1.0, 1.0), glm::vec4 backColor = glm::vec4(0.2, 0.2, 0.2, 0.4), bool isDebugInfo = false);

    virtual void drawUI_Element();

    virtual int onKeyDown(SDL_Keycode key);

    void setFilter(std::string filter)
    {
        regExp->assign(filter);
    }

    std::regex* regExp = new std::regex(".");
    int maxLength = 20;
    std::string text;

private:
    int labelOffsetX, labelOffsetY;
    

};

