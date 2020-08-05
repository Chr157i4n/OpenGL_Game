#include "UI_Element_Label.h"
#include "UI.h"

UI_Element_Label::UI_Element_Label(int x, int y, int w, int h, std::string label, uint64 lifespan, int fontSize, glm::vec4 foreColor, glm::vec4 backColor, bool isDebugInfo) : UI_Element(x, y, w, h, lifespan, foreColor, backColor, isDebugInfo)
{
    this->text = label;
    this->fontSize = fontSize;
}

void UI_Element_Label::drawUI_Element()
{
    UI::drawString(x, y, text, foreColor);
}


std::string UI_Element_Label::getText() const
{
    return text;
}

void UI_Element_Label::setText(std::string text)
{
    this->text = text;
}

int UI_Element_Label::getFontSize() const
{
    return fontSize;
}

void UI_Element_Label::setFontSize(int fontSize)
{
    this->fontSize = fontSize;
}

