#include "UI_Element.h"

UI_Element::UI_Element()
{
    startTimestamp = SDL_GetPerformanceCounter() / SDL_GetPerformanceFrequency();
}


int UI_Element::getX() const
{
    return x;
}

void UI_Element::setX(int x)
{
    this->x = x;
}

int UI_Element::getY() const
{
    return y;
}

void UI_Element::setY(int y)
{
    this->y = y;
}

int UI_Element::getW() const
{
    return w;
}

void UI_Element::setW(int w)
{
    this->w = w;
}

int UI_Element::getH() const
{
    return h;
}

void UI_Element::setH(int h)
{
    this->h = h;
}

glm::vec4 UI_Element::getColor() const
{
    return color;
}

void UI_Element::setColor(glm::vec4 color)
{
    this->color = color;
}

bool UI_Element::getIsDebugInfo() const
{
    return isDebugInfo;
}

void UI_Element::setIsDebugInfo(bool isDebugInfo)
{
    this->isDebugInfo = isDebugInfo;
}


UI_Element_Type UI_Element::getUielementtype() const
{
    return ui_element_type;
}

void UI_Element::setUielementtype(UI_Element_Type uielementtype)
{
    ui_element_type = uielementtype;
}


uint64 UI_Element::getLifespan() const
{
    return lifespan;
}

void UI_Element::setLifespan(uint64 lifespan)
{
    this->lifespan = lifespan;
}

bool UI_Element::isStillAlive()
{
    uint64 timestamp = SDL_GetPerformanceCounter()/SDL_GetPerformanceFrequency();

    if (lifespan == 0)
        return true;
    else if (lifespan == -1)
        return false;
    else if (timestamp > startTimestamp + lifespan)
        return false;
    else
        return true;
}


int UI_Element::getID() const
{
    return ID;
}

void UI_Element::setID(int ID)
{
    this->ID = ID;
}

