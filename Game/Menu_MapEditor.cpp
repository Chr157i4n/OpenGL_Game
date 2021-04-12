#include "Menu_MapEditor.h"

#include "Game.h"
#include "UI.h"

Menu_MapEditor::Menu_MapEditor()
{
	pB_start = new UI_Element_Button(10, 130, 200, 50, 0, "Starten");
	pB_start->setCallback([&] { Game::startGame(Game_Mode::GameMode_MapEditor); return 2; });
	addMenuElement(pB_start);

	pB_back = new UI_Element_Button(10, 10, 200, 50, 0, "Zurueck");
	pB_back->setCallback([&] { Game::toggleSubMenu(MenuType::MENU_MAPEDITOR); return 0; });
	addMenuElement(pB_back);

	tE_mapName = new UI_Element_TextEdit(500, 10, 200, 50, "Map Name", 0);
	//tE_mapName->setFilter("([0-9]|[.])");
	tE_mapName->setCallback([&] { return 0; });
	addMenuElement(tE_mapName);
}