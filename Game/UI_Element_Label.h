#pragma once
#include "defines.h"

#include <string>

#include "UI_Element.h"

class UI_Element_Label	: public UI_Element
{
public:
    UI_Element_Label(int x, int y, int w, int h, std::string label, uint64 lifespan, int fontSize = 1, glm::vec4 foreColor = glm::vec4(1.0, 1.0, 1.0, 1.0), glm::vec4 backColor = glm::vec4(0.2, 0.2, 0.2, 0.4), bool isDebugInfo = false);

    virtual void drawUI_Element();

    std::string getText() const;
    void setText(std::string text);

    int getFontSize() const;
    void setFontSize(int fontSize);

protected:
	std::string text = "";
	int fontSize=1;

};

