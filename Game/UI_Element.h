#pragma once
#include "defines.h"

#include "libs/glm/glm.hpp"
#include <SDL.h>

enum UI_Element_Type {
    label,
    progressbar,
};

class UI_Element
{
public:
    UI_Element();

    virtual void drawUI_Element()=0;

protected:
	int x = 0, y = 0, w = 0, h = 0;
	glm::vec4 color = glm::vec4(1, 1, 1, 1);
	bool isDebugInfo = false;
    UI_Element_Type ui_element_type;
    uint64 lifespan = 0;
    uint64 startTimestamp = 0;
    int ID = -1;
    bool isSelected = false;

public:
    int getX() const;
    void setX(int x);

    int getY() const;
    void setY(int y);

    int getW() const;
    void setW(int w);

    int getH() const;
    void setH(int h);

    glm::vec4 getColor() const;
    void setColor(glm::vec4 color);

    bool getIsDebugInfo() const;
    void setIsDebugInfo(bool isDebugInfo);

    UI_Element_Type getUielementtype() const;
    void setUielementtype(UI_Element_Type uielementtype);

    uint64 getLifespan() const;
    void setLifespan(uint64 lifespan);

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

};

