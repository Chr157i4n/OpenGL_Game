#include "Menu_Main.h"

#include <algorithm>
#include <string>
#include <vector>

#include "Game.h"
#include "UI.h"
#include "Localization.h"

Menu_Main::Menu_Main()
{
	setLayout(MenuLayout(3, 200, 50));

	pB_start_singleplayer = new UI_Element_Button(0, 0, 200, 50, 0, Locale::get("menu.singleplayer"));
	pB_start_singleplayer->setCallback([&] { startSinglePlayer(ConfigManager::level); return MENU_CONSUMED; });
	addMenuElement(pB_start_singleplayer, 0, 0);

	pB_start_multiplayer = new UI_Element_Button(0, 0, 200, 50, 0, Locale::get("menu.multiplayer"));
	pB_start_multiplayer->setCallback([&] { Game::toggleSubMenu(MenuType::MENU_MULTIPLAYER); return MENU_CONSUMED; });
	addMenuElement(pB_start_multiplayer, 1, 0);

	pB_start_mapeditor = new UI_Element_Button(0, 0, 200, 50, 0, Locale::get("menu.mapeditor"));
	pB_start_mapeditor->setCallback([&] { Game::toggleSubMenu(MenuType::MENU_MAPEDITOR); return MENU_CONSUMED; });
	addMenuElement(pB_start_mapeditor, 2, 0);

	pB_options = new UI_Element_Button(0, 0, 200, 50, 0, Locale::get("menu.options"));
	pB_options->setCallback([&] { Game::toggleSubMenu(MenuType::MENU_OPTIONS); return MENU_CONSUMED; });
	addMenuElement(pB_options, 3, 0);

	pB_exit = new UI_Element_Button(0, 0, 200, 50, 0, Locale::get("menu.exit"));
	pB_exit->setCallback([&] { Game::quit(); return MENU_OK; });
	addMenuElement(pB_exit, 4, 0);

	dD_maps = new UI_Element_Dropdown(0, 0, 200, 50, 0);
	dD_maps->setName(Locale::get("menu.startmap"));

	std::vector<std::string> maps = ResourceManager::readAllMaps();
	if (maps.empty())
		dD_maps->setName(Locale::get("menu.noMaps"));

	for (std::string map : maps)
	{
		UI_Element_Dropdown_Item* mapItem = new UI_Element_Dropdown_Item();

		// readAllMaps returns full paths like "levels/test.json"; only the bare
		// name is shown. Map::load re-adds the folder and extension itself.
		std::vector<std::string> toDeletes;
		toDeletes.push_back("levels/");
		toDeletes.push_back(".json");

		for (std::string toDelete : toDeletes)
		{
			size_t pos = map.find(toDelete);
			if (pos != std::string::npos)
			{
				// If found then erase it from string
				map.erase(pos, toDelete.length());
			}
		}

		mapItem->label = map;
		mapItem->callback = [&] { startSinglePlayer(map); return MENU_CONSUMED; };
		dD_maps->addItem(mapItem);
	}
	addMenuElement(dD_maps, 0, 1);

	tE_playername = new UI_Element_TextEdit(0, 0, 200, 50, ConfigManager::player_name, 0);
	tE_playername->setCallback([&] { setPlayerName(); return MENU_OK; });
	addMenuElement(tE_playername, 0, 2);
}

void Menu_Main::drawMenu()
{
	Renderer::drawMainMenuBackground();

	// A short help line under the grid explaining the keyboard controls.
	applyLayout();
	std::string hint = Locale::get("menu.hint");

	int lowestY = Game::getWindowHeight();
	for (UI_Element* element : menuElementList)
		lowestY = std::min(lowestY, element->getY());

	UI::drawString(10, std::max(10, lowestY - 30), hint, glm::vec4(1, 1, 1, 0.5));

	Menu::drawMenu();
}

void Menu_Main::startSinglePlayer(std::string mapname)
{
	if (mapname == "")
	{
		UI_Element_Dropdown_Item* selectedMap = dD_maps->getSelectedItem();
		if (selectedMap)
			mapname = selectedMap->label;
	}

	if (mapname == "")
		mapname = ConfigManager::level;

	Map::load(mapname);
	Game::startGame(Game_Mode::GameMode_SinglePlayer);
}

void Menu_Main::setPlayerName()
{
	ConfigManager::player_name = tE_playername->text;
}
