#pragma once
#include "defines.h"

#include <glm/glm.hpp>
#include <functional>
#include <chrono>
#include <SDL.h>

enum UI_Element_Type {
    label,
    progressbar,
};

class UI_Element
{
public:
    UI_Element(int x, int y, int w, int h, float lifespan=0, glm::vec4 foreColor = glm::vec4(1.0, 1.0, 1.0, 1.0) , glm::vec4 backColor = glm::vec4(0.2, 0.2, 0.2, 0.4) , bool isDebugInfo=false);

    virtual void drawUI_Element()=0;

    int getX() const;
    void setX(int x);

    int getY() const;
    void setY(int y);

    int getW() const;
    void setW(int w);

    int getH() const;
    void setH(int h);

    glm::vec4 getForeColor() const;
    void setForeColor(glm::vec4 color);

    glm::vec4 getBackColor() const;
    void setBackColor(glm::vec4 color);

    bool getIsDebugInfo() const;
    void setIsDebugInfo(bool isDebugInfo);

    UI_Element_Type getUielementtype() const;
    void setUielementtype(UI_Element_Type uielementtype);

    float getLifespan() const;
    void setLifespan(float lifespan);

    bool isStillAlive();

    int getID() const;
    void setID(int ID);

    bool getIsSelected()
    {
        return isSelected;
    }

    void setIsSelected(bool isSelected)
    {
        this->isSelected = isSelected;
    }

    virtual bool isMouseOver(float mouseX, float mouseY);

    void setCallback(std::function<int()> callback)
    {
        this->callback = callback;
    }

    int callCallBack(SDL_MouseButtonEvent* buttonEvent = nullptr);

    virtual int onMouseClick(float mouseX, float mouseY, SDL_MouseButtonEvent* buttonEvent = nullptr);

    virtual int onMouseDrag(float mouseX, float mouseY, SDL_MouseButtonEvent* buttonEvent = nullptr);

    virtual int onKeyDown(SDL_Keycode key);

    virtual int increase();

    virtual int decrease();

protected:
	int x, y, w, h;

	glm::vec4 foreColor;
    glm::vec4 backColor;

	bool isDebugInfo = false;
    UI_Element_Type ui_element_type;


    float lifespan;
    std::chrono::system_clock::time_point startTimestamp;

    int ID = -1;
    bool isSelected = false;

    bool singleAction = false;
    std::function<int()> callback;

 

};

