#pragma once
#include "defines.h"

#include "libs/glm/glm.hpp"
#include <functional>
#include <chrono>

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

    bool isMouseOver(float mouseX, float mouseY);

    void setCallback(std::function<void()> callback)
    {
        this->callback = callback;
    }

    void callCallBack()
    {
        if(callback != nullptr)
            callback();
    }

    virtual void action(float mouseX, float mouseY);

    virtual void increase();

    virtual void decrease();

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

    std::function<void()> callback;

 

};

