#include "UI_Element.h"

#include "Game.h"

UI_Element::UI_Element(int x, int y, int w, int h, float lifespan, glm::vec4 foreColor, glm::vec4 backColor, bool isDebugInfo)
{
    startTimestamp = std::chrono::system_clock::now();

    this->x = x;
    this->y = y;
    this->w = w;
    this->h = h;
    this->isDebugInfo = isDebugInfo;
    this->lifespan = lifespan;
    this->foreColor = foreColor;
    this->backColor = backColor;
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

glm::vec4 UI_Element::getForeColor() const
{
    return foreColor;
}

void UI_Element::setForeColor(glm::vec4 color)
{
    this->foreColor = color;
}

glm::vec4 UI_Element::getBackColor() const
{
    return backColor;
}

void UI_Element::setBackColor(glm::vec4 color)
{
    this->backColor = color;
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


float UI_Element::getLifespan() const
{
    return lifespan;
}

void UI_Element::setLifespan(float lifespan)
{
    this->lifespan = lifespan;
}

bool UI_Element::isStillAlive()
{
    if (lifespan == 0)
        return true;
    else if (lifespan == -1)
        return false;

    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    std::chrono::duration<double, std::milli> duration = now - startTimestamp;


    if (duration.count() > lifespan)
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

bool UI_Element::isMouseOver(float mouseX, float mouseY)
{
    return (mouseX > this->getX() && mouseX < this->getX() + this->getW() && Game::getWindowHeight() - mouseY > this->getY() && Game::getWindowHeight() - mouseY < this->getY() + this->getH());
}

int UI_Element::callCallBack(SDL_MouseButtonEvent* buttonEvent)
{
    if (callback == nullptr) return -1;
    //if (buttonEvent->clicks > 1 && singleAction) return;
    return callback();
}

int UI_Element::onMouseClick(float mouseX, float mouseY, SDL_MouseButtonEvent* buttonEvent)
{
    return 0;
}

int UI_Element::onMouseDrag(float mouseX, float mouseY, SDL_MouseButtonEvent* buttonEvent)
{
    return 0;
}

int UI_Element::onKeyDown(SDL_Keycode key)
{
    return 0;
}

int UI_Element::increase()
{
    return 0;
}

int UI_Element::decrease()
{
    return 0;
}
