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

// Return codes of the UI event handlers, replacing the magic numbers the
// menus used to return.
enum MenuResult {
    MENU_OK = 0,       // event was handled, the mouse press may still be recorded
    MENU_CONSUMED = 2, // event was fully handled, do not record the mouse press
};

class UI_Element
{
public:
    UI_Element(int x, int y, int w, int h, float lifespan=0, glm::vec4 foreColor = glm::vec4(1.0, 1.0, 1.0, 1.0) , glm::vec4 backColor = glm::vec4(0.2, 0.2, 0.2, 0.4) , bool isDebugInfo=false);

    virtual ~UI_Element();

    virtual void drawUI_Element()=0;

    int getX() const;
    void setX(int x);

    int getY() const;
    void setY(int y);

    int getW() const;
    void setW(int w);

    int getH() const;
    void setH(int h);

    // Sets position and size in one go (used by the menu grid layout).
    void setPosition(int x, int y, int w, int h);

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

    virtual int callCallBack(SDL_MouseButtonEvent* buttonEvent = nullptr);

    virtual int onMouseClick(float mouseX, float mouseY, SDL_MouseButtonEvent* buttonEvent = nullptr);

    virtual int onMouseDrag(float mouseX, float mouseY, SDL_MouseButtonEvent* buttonEvent = nullptr);

    virtual int onKeyDown(SDL_Keycode key);

    virtual int increase();

    virtual int decrease();

    // Navigates the element's own content (dropdown items, states...). Returns
    // 0 when the element has nothing to navigate, so the menu can fall back to
    // selecting the next element. direction is +1 (down) or -1 (up).
    virtual int navigateMenu(int direction);

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

