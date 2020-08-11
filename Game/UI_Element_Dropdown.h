#pragma once
#include "defines.h"

#include <string>
#include <vector>

#include "UI_Element.h"

struct UI_Element_Dropdown_Item
{
	std::string label="";
	int ID=-1;
	int labelOffsetX, labelOffsetY;
	bool isSelected = false;
	std::function<int()> callback;
};

class UI_Element_Dropdown : public UI_Element
{
public:
	UI_Element_Dropdown(int x, int y, int w, int h, uint64 lifespan, glm::vec4 foreColor = glm::vec4(1, 1, 1, 1), glm::vec4 backColor = glm::vec4(0.2, 0.2, 0.2, 0.4), bool isDebugInfo = false);

	virtual void drawUI_Element();

	void addItem(UI_Element_Dropdown_Item* item);

	UI_Element_Dropdown_Item* getSelectedItem()
	{
		return selectedItem;
	}

	void close()
	{
		dropdown_open = false;
	}

	void open()
	{
		dropdown_open = true;
	}

	void toggleOpen()
	{
		dropdown_open = !dropdown_open;
	}

	bool isMouseOver(float mouseX, float mouseY);

	virtual int onMouseClick(float mouseX, float mouseY, SDL_MouseButtonEvent* buttonEvent = nullptr);

	void setName(std::string newName)
	{
		this->name = newName;
	}

private:
	int fontSize = 1;
	std::vector<UI_Element_Dropdown_Item*> items;
	UI_Element_Dropdown_Item* selectedItem;
	bool dropdown_open = false;
	std::string name = "";

};

