#include "MenuLayout.h"

#include <algorithm>

#include "Game.h"

MenuLayout::MenuLayout(int columns, int cellWidth, int cellHeight, int gap, int margin)
	: requestedColumns(columns), baseCellWidth(cellWidth), baseCellHeight(cellHeight), gap(gap), margin(margin)
{
}

MenuLayoutResult MenuLayout::compute(const std::vector<MenuGridSlot>& requested) const
{
	MenuLayoutResult result;

	int windowWidth = Game::getWindowWidth();
	int windowHeight = Game::getWindowHeight();

	int maxCol = 0;
	int maxRow = 0;
	for (const MenuGridSlot& slot : requested)
	{
		maxCol = std::max(maxCol, slot.col);
		maxRow = std::max(maxRow, slot.row);
	}

	// How many columns actually fit into the window.
	int columns = std::min(requestedColumns, maxCol + 1);
	int cellWidth = baseCellWidth;
	while (columns > 1 && columns * cellWidth + (columns - 1) * gap + 2 * margin > windowWidth)
	{
		columns--;
	}

	int availableWidth = windowWidth - 2 * margin - (columns - 1) * gap;
	cellWidth = std::max(120, std::min(cellWidth, availableWidth / columns));

	// Wrap elements that do not fit into the resolved columns.
	int rows = 0;
	for (const MenuGridSlot& slot : requested)
	{
		MenuGridSlot wrapped;
		wrapped.col = slot.col % columns;
		wrapped.row = slot.row + (slot.col / columns) * (maxRow + 1);

		rows = std::max(rows, wrapped.row + 1);
		result.slots.push_back(wrapped);
	}

	int availableHeight = windowHeight - 2 * margin - (rows - 1) * gap;
	int cellHeight = std::max(30, std::min(baseCellHeight, availableHeight / rows));

	result.cellWidth = cellWidth;
	result.cellHeight = cellHeight;

	return result;
}

void MenuLayout::cellPosition(const MenuLayoutResult& result, const MenuGridSlot& slot, int& x, int& y) const
{
	x = margin + slot.col * (result.cellWidth + gap);
	y = Game::getWindowHeight() - margin - slot.row * (result.cellHeight + gap) - result.cellHeight;
}
