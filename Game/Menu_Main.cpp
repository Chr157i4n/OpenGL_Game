#include "Menu_Main.h"

#include "Game.h"
#include "UI.h"

Menu_Main::Menu_Main()
{
	UI_Element_Button* pB_resume = new UI_Element_Button(10, 130, 200, 50, 0, "Start");
	//pB_resume->setCallback([&] { Game::toggleMenu(); });
	addMenuElement(pB_resume);

	UI_Element_Button* pB_options = new UI_Element_Button(10, 70, 200, 50, 0, "Optionen");
	addMenuElement(pB_options);

	UI_Element_Button* pB_exit = new UI_Element_Button(10, 10, 200, 50, 0, "Beenden");
	pB_exit->setCallback([&] { Game::quit(); });
	addMenuElement(pB_exit);
}
