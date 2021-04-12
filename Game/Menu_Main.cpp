#include "Menu_Main.h"

#include "Game.h"
#include "UI.h"

Menu_Main::Menu_Main()
{
	pB_start_singleplayer = new UI_Element_Button(10, 250, 200, 50, 0, "Einzelspieler");
	pB_start_singleplayer->setCallback([&] { startSinglePlayer(ConfigManager::level); return 2; });
	addMenuElement(pB_start_singleplayer);

	pB_start_multiplayer = new UI_Element_Button(10, 190, 200, 50, 0, "Mehrspieler");
	pB_start_multiplayer->setCallback([&] { Game::toggleSubMenu(MenuType::MENU_MULTIPLAYER); return 2; });
	addMenuElement(pB_start_multiplayer);

	pB_start_mapeditor = new UI_Element_Button(10, 130, 200, 50, 0, "Mapeditor");
	pB_start_mapeditor->setCallback([&] { Game::toggleSubMenu(MenuType::MENU_MAPEDITOR); return 2; });
	addMenuElement(pB_start_mapeditor);

	pB_options = new UI_Element_Button(10, 70, 200, 50, 0, "Optionen");
	pB_options->setCallback([&] { Game::toggleSubMenu(MenuType::MENU_OPTIONS); return 0; });
	addMenuElement(pB_options);

	pB_exit = new UI_Element_Button(10, 10, 200, 50, 0, "Beenden");
	pB_exit->setCallback([&] { Game::quit(); return 0; });
	addMenuElement(pB_exit);

	dD_maps = new UI_Element_Dropdown(500, Game::getWindowHeight() - 60, 400, 50, 0);
	dD_maps->setName("Map starten");
	std::vector<std::string> maps = ResourceManager::readAllMaps();
	for (std::string map : maps)
	{
		UI_Element_Dropdown_Item* mapItem = new UI_Element_Dropdown_Item();

		std::vector<std::string> toDeletes;
		toDeletes.push_back("levels\\");
		toDeletes.push_back(".xml");

		for(std::string toDelete : toDeletes)
		{
			size_t pos = map.find(toDelete);
			if (pos != std::string::npos)
			{
				// If found then erase it from string
				map.erase(pos, toDelete.length());
			}
		}
		mapItem->label = map;
		mapItem->callback = [&] { startSinglePlayer(); return 2; };
		dD_maps->addItem(mapItem);
	}
	addMenuElement(dD_maps);

	tE_playername = new UI_Element_TextEdit(500, 10, 200, 50, ConfigManager::player_name, 0);
	tE_playername->setCallback([&] { setPlayerName(); return 0; });
	addMenuElement(tE_playername);
}

void Menu_Main::drawMenu()
{
	Renderer::drawMainMenuBackground();

	this->Menu::drawMenu();
}

void Menu_Main::startSinglePlayer(std::string mapname)
{
	if(mapname=="")
		mapname = dD_maps->getSelectedItem()->label;

	Map::load(mapname);
	Game::startGame(Game_Mode::GameMode_SinglePlayer);
}

void Menu_Main::setPlayerName()
{
	ConfigManager::player_name = tE_playername->text;
}

