#include "UI_Element_Label.h"
#include "UI.h"

UI_Element_Label::UI_Element_Label(int x, int y, std::string label, uint64 lifespan, int fontSize, glm::vec4 color,  bool isDebugInfo) : UI_Element()
{
    this->text = label;
    this->x = x;
    this->y = y;
    this->isDebugInfo = isDebugInfo;
    this->fontSize = fontSize;
    this->color = color;
    this->lifespan = lifespan;
}

void UI_Element_Label::drawUI_Element()
{
    UI::drawString(x, y, text, color);
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

