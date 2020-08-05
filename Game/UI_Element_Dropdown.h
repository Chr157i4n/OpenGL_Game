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
};

class UI_Element_Dropdown : public UI_Element
{
public:
	UI_Element_Dropdown(int x, int y, int w, int h, uint64 lifespan, glm::vec4 foreColor = glm::vec4(1, 1, 1, 1), glm::vec4 backColor = glm::vec4(0.2, 0.2, 0.2, 0.4), bool isDebugInfo = false);

	virtual void drawUI_Element();

	void addItem(UI_Element_Dropdown_Item* item);

private:
	int fontSize = 1;
	std::vector<UI_Element_Dropdown_Item*> items;
	UI_Element_Dropdown_Item* selectedItem;

};

