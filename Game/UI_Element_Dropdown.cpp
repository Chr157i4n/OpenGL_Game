#include "UI_Element_Dropdown.h"

#include "Game.h"

UI_Element_Dropdown::UI_Element_Dropdown(int x, int y, int w, int h, uint64 lifespan, glm::vec4 foreColor, glm::vec4 backColor, bool isDebugInfo) : UI_Element(x, y, w, h, lifespan, foreColor, backColor, isDebugInfo)
{
	UI_Element_Dropdown_Item* test1 = new UI_Element_Dropdown_Item();	
	test1->label = "test1";
	addItem(test1);

	UI_Element_Dropdown_Item* test2 = new UI_Element_Dropdown_Item();
	test2->label = "test2";
	addItem(test2);

	UI_Element_Dropdown_Item* test3 = new UI_Element_Dropdown_Item();
	test3->label = "test3";
	addItem(test3);


}

void UI_Element_Dropdown::drawUI_Element()
{
	float outlineThickness = 0;//0.01;


	//box of selected item
	glColor4f(backColor.r, backColor.g, backColor.b, backColor.a);
	glBegin(GL_QUADS);
	glVertex2f(UI::to_x_gl(x),		UI::to_y_gl(y));
	glVertex2f(UI::to_x_gl(x + w),	UI::to_y_gl(y));
	glVertex2f(UI::to_x_gl(x + w),	UI::to_y_gl(y + h));
	glVertex2f(UI::to_x_gl(x),		UI::to_y_gl(y + h));
	glEnd();


	//outline of all items
	glColor4f(backColor.r * 1.5, backColor.g * 1.5, backColor.b*1.5, backColor.a);
	glBegin(GL_QUADS);
	glVertex2f(UI::to_x_gl(x + w),				UI::to_y_gl(y));
	glVertex2f(UI::to_x_gl(x + 1.0 / 4.0 * w),	UI::to_y_gl(y));
	glVertex2f(UI::to_x_gl(x + 1.0 / 4.0 * w),	UI::to_y_gl(y - h * items.size()));
	glVertex2f(UI::to_x_gl(x + w),				UI::to_y_gl(y - h * items.size()));
	glEnd();


	for (int i = 0; i < items.size(); i++)
	{

		float alpha = 1;
		if (!items[i]->isSelected)
		{
			alpha = backColor.a;
		}

		//outline of single item
		glColor4f(backColor.r*2, backColor.g*2, backColor.b*2, alpha);
		glBegin(GL_QUADS);
		glVertex2f(UI::to_x_gl(x + w),					UI::to_y_gl(y - (i) * h));
		glVertex2f(UI::to_x_gl(x + 1.0 / 4.0 * w),		UI::to_y_gl(y - (i) * h));
		glVertex2f(UI::to_x_gl(x + 1.0 / 4.0 * w),		UI::to_y_gl(y - (i + 1) * h));
		glVertex2f(UI::to_x_gl(x + w),					UI::to_y_gl(y - (i + 1) * h));
		
		
		glEnd();

		//test of single item
		UI::drawString(x + items[i]->labelOffsetX, y - (i+1)*h + items[i]->labelOffsetY, items[i]->label, foreColor);
	}
}

void UI_Element_Dropdown::addItem(UI_Element_Dropdown_Item* item)
{
	item->ID = items.size();
	item->labelOffsetX = w / 2 - 6 * item->label.size();		//todo: pixelsize of character are different. calculate the offset correctly
	item->labelOffsetY = h / 2 - 10;

	items.push_back(item);
}
