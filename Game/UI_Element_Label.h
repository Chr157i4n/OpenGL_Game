#pragma once
#include "defines.h"

#include <string>

#include "UI_Element.h"

class UI_Element_Label	: public UI_Element
{
public:
    UI_Element_Label(int x, int y, std::string label, uint64 lifespan, int fontSize = 1, glm::vec4 color = glm::vec4(1,1,1,1), bool isDebugInfo = false);

    virtual void drawUI_Element();

private:
	std::string text = "";
	int fontSize=1;
public:
    std::string getText() const;
    void setText(std::string text);

    int getFontSize() const;
    void setFontSize(int fontSize);

};

