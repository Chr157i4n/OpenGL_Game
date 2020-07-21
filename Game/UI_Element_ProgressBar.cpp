#include "UI_Element_ProgressBar.h"

UI_Element_ProgressBar::UI_Element_ProgressBar(int x, int y, int w, int h, float value, uint64 lifespan, bool isDebugInfo) : UI_Element()
{
    this->value = value;
    this->x = x;
    this->y = y;
    this->w = w;
    this->h = h;
    this->isDebugInfo = isDebugInfo;
    this->lifespan = lifespan;
}

void UI_Element_ProgressBar::drawUI_Element()
{

}


float UI_Element_ProgressBar::getValue() const
{
    return value;
}

void UI_Element_ProgressBar::setValue(float value)
{
    this->value = value;
}

