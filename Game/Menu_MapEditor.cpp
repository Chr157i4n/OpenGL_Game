#include "Menu_MapEditor.h"

#include "Game.h"
#include "UI.h"
#include "Localization.h"

Menu_MapEditor::Menu_MapEditor()
{
	setLayout(MenuLayout(2, 200, 50));

	pB_start = new UI_Element_Button(0, 0, 200, 50, 0, Locale::get("menu.editorstart"));
	pB_start->setCallback([&] { Game::startGame(Game_Mode::GameMode_MapEditor); return MENU_CONSUMED; });
	addMenuElement(pB_start, 0, 0);

	pB_back = new UI_Element_Button(0, 0, 200, 50, 0, Locale::get("menu.back"));
	pB_back->setCallback([&] { Game::toggleSubMenu(MenuType::MENU_MAPEDITOR); return MENU_CONSUMED; });
	addMenuElement(pB_back, 1, 0);

	tE_mapName = new UI_Element_TextEdit(0, 0, 200, 50, Locale::get("menu.mapname"), 0);
	//tE_mapName->setFilter("([0-9]|[.])");
	tE_mapName->setCallback([&] { return MENU_OK; });
	addMenuElement(tE_mapName, 0, 1);
}
