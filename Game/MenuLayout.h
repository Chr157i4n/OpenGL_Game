#pragma once
#include "defines.h"

#include <vector>

// Grid slot of a menu element. row 0 is the top row, col 0 the left column.
struct MenuGridSlot
{
	int row = 0;
	int col = 0;
};

// Result of resolving the requested grid against the current window size.
struct MenuLayoutResult
{
	std::vector<MenuGridSlot> slots;	// wrapped slots, one per menu element
	int cellWidth = 200;
	int cellHeight = 50;
};

// Computes pixel positions for menu elements from a logical grid instead of
// hard coded coordinates. The grid degrades gracefully: if the window is too
// narrow for the requested number of columns, the extra columns are wrapped
// into additional rows and the cells are shrunk.
class MenuLayout
{
public:
	MenuLayout(int columns = 2, int cellWidth = 200, int cellHeight = 50, int gap = 10, int margin = 10);

	MenuLayoutResult compute(const std::vector<MenuGridSlot>& requested) const;

	// Bottom left corner of a cell in window pixel coordinates (y grows
	// upwards), matching every other UI_Element.
	void cellPosition(const MenuLayoutResult& result, const MenuGridSlot& slot, int& x, int& y) const;

private:
	int requestedColumns;
	int baseCellWidth;
	int baseCellHeight;
	int gap;
	int margin;
};
