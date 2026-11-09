#pragma once
#include "defines.h"

#include <string>
#include <vector>

#include "UI_Element.h"

struct UI_Element_Dropdown_Item
{
	std::string label = "";
	int ID = -1;
	int labelOffsetX = 0, labelOffsetY = 0;
	bool isSelected = false;
	std::function<int()> callback;
};

class UI_Element_Dropdown : public UI_Element
{
public:
	UI_Element_Dropdown(int x, int y, int w, int h, uint64 lifespan, glm::vec4 foreColor = glm::vec4(1, 1, 1, 1), glm::vec4 backColor = glm::vec4(0.2, 0.2, 0.2, 0.4), bool isDebugInfo = false);

	virtual ~UI_Element_Dropdown()
	{
		clearItems();
	}

	virtual void drawUI_Element();

	void addItem(UI_Element_Dropdown_Item* item);

	void clearItems()
	{
		for (UI_Element_Dropdown_Item* item : items)
			delete item;

		items.clear();
		selectedItem = nullptr;
		dropdown_open = false;
	}

	UI_Element_Dropdown_Item* getSelectedItem()
	{
		return selectedItem;
	}

	int getSelectedIndex() const
	{
		return selectedItem ? selectedItem->ID : -1;
	}

	int getItemCount() const
	{
		return (int)items.size();
	}

	bool isOpen() const
	{
		return dropdown_open;
	}

	void close()
	{
		dropdown_open = false;
	}

	void open()
	{
		updateOpenDirection();
		dropdown_open = true;
	}

	void toggleOpen()
	{
		if (dropdown_open)
			close();
		else
			open();
	}

	bool isMouseOver(float mouseX, float mouseY);

	virtual int onMouseClick(float mouseX, float mouseY, SDL_MouseButtonEvent* buttonEvent = nullptr);

	// Keyboard support: the list can be opened and traversed with W/S or A/D.
	virtual int navigateMenu(int direction);

	virtual int increase();

	virtual int decrease();

	// Enter/Space: opens the list, or activates the currently selected item.
	virtual int callCallBack(SDL_MouseButtonEvent* buttonEvent = nullptr);

	void setName(std::string newName)
	{
		this->name = newName;
	}

private:
	void selectItem(UI_Element_Dropdown_Item* item);

	// The list opens downwards by default, upwards when there is more free
	// space above the box than below it.
	void updateOpenDirection();

	int dir;

	int fontSize = 1;
	std::vector<UI_Element_Dropdown_Item*> items;
	UI_Element_Dropdown_Item* selectedItem = nullptr;
	bool dropdown_open = false;
	std::string name = "";

};
