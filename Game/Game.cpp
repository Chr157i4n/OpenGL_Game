#include "Game.h"



#define STB_IMAGE_IMPLEMENTATION
#include "libs/stb_image.h"
#undef STB_IMAGE_IMPLEMENTATION


SDL_Window* Game::window;

std::vector<std::shared_ptr<Object>> Game::map;
std::vector<std::shared_ptr<Object>> Game::objects;
std::vector< std::shared_ptr<Character> > Game::characters;
std::vector< std::shared_ptr<Player> > Game::players;
std::vector< std::shared_ptr<Character> > Game::clients;
std::vector< std::shared_ptr<NPC> > Game::npcs;
std::vector< std::shared_ptr<Bullet> > Game::bullets;


bool Game::pressedKeys[300];
bool Game::pressedMouseButtons[6];

float32  Game::FPS = 0;
bool  Game::close = false;



float32 Game::delta = 0;

bool Game::showInfo = false;
GameState Game::gameState = GameState::GAME_MENU;

bool Game::showShadowMap = false;
bool Game::showMap = false;

Menu* Game::menu_Main = nullptr;
Menu* Game::menu_Pause = nullptr;
Menu* Game::menu_Options = nullptr;
Menu* Game::menu_Multiplayer = nullptr;
Menu* Game::menu_MapEditor = nullptr;

Menu* Game::menu_current = menu_Main;
Menu* Game::menu_last = menu_Main;

UI_Element_Graph* Game::fpsGraph;
UI_Element_Label* Game::lbl_stopwatch1, * Game::lbl_stopwatch2, * Game::lbl_stopwatch3, * Game::lbl_stopwatch4, * Game::lbl_stopwatch5, * Game::lbl_stopwatch6;
UI_Element_Label* Game::lbl_ObjectCount;

StopWatch Game::gameStopWatch;
StopWatch  Game::frametimeStopWatch;

std::vector<Font3D*> Game::texts;

GameMode* Game::gameMode;



/// <summary>
/// this function inits everything before the game can be started
/// </summary>
void Game::init()
{

	Renderer::initOpenGL();
	Renderer::loadShader();

	LuaManager::init();
	LuaManager::loadScripts();

	AudioManager::init();

	UI::init();

	fpsGraph = new UI_Element_Graph(10, getWindowHeight() * 3 / 4, 100, 100, 0, glm::vec4(0, 0, 1, 1), glm::vec4(0.2, 0.2, 0.2, 0.4), true);
	UI::addElement(fpsGraph);

	lbl_stopwatch1 = new UI_Element_Label(10, 50, 100, 50, "", 0, 1, glm::vec4(1, 1, 1, 1), glm::vec4(0, 0, 0, 0.2), true);
	UI::addElement(lbl_stopwatch1);

	lbl_stopwatch2 = new UI_Element_Label(10, 80, 100, 50, "", 0, 1, glm::vec4(1, 1, 1, 1), glm::vec4(0, 0, 0, 0.2), true);
	UI::addElement(lbl_stopwatch2);

	lbl_stopwatch3 = new UI_Element_Label(10, 110, 100, 50, "", 0, 1, glm::vec4(1, 1, 1, 1), glm::vec4(0, 0, 0, 0.2), true);
	UI::addElement(lbl_stopwatch3);

	lbl_stopwatch4 = new UI_Element_Label(10, 140, 100, 50, "", 0, 1, glm::vec4(1, 1, 1, 1), glm::vec4(0, 0, 0, 0.2), true);
	UI::addElement(lbl_stopwatch4);

	lbl_stopwatch5 = new UI_Element_Label(10, 170, 100, 50, "", 0, 1, glm::vec4(1, 1, 1, 1), glm::vec4(0, 0, 0, 0.2), true);
	UI::addElement(lbl_stopwatch5);

	lbl_stopwatch6 = new UI_Element_Label(10, 200, 100, 50, "", 0, 1, glm::vec4(1, 1, 1, 1), glm::vec4(0, 0, 0, 0.2), true);
	UI::addElement(lbl_stopwatch6);

	lbl_ObjectCount = new UI_Element_Label(10, 230, 100, 50, "", 0, 1, glm::vec4(1, 1, 1, 1), glm::vec4(0, 0, 0, 0.2), true);
	UI::addElement(lbl_ObjectCount);


	Renderer::drawLoadingScreen();

	//std::vector<std::string> maps =	ResourceManager::readAllMaps();

	//std::string levelname = ConfigManager::level;
	//Map::load("level_farm");

	menu_Main = new Menu_Main();
	menu_Pause = new Menu_Pause();
	menu_Options = new Menu_Options();
	menu_Multiplayer = new Menu_Multiplayer();
	menu_MapEditor = new Menu_MapEditor();

	menu_current = menu_Main;
	gameState = GameState::GAME_MENU;
	gameMode = new GameMode_Menu();

	Renderer::init();	


	gameLoop();
}


/// <summary>
/// this methods starts the game
/// </summary>
void Game::startGame(Game_Mode game_Mode)
{
	delta = 0.0f;
	gameState = GameState::GAME_ACTIVE;

	delete gameMode;
	switch (game_Mode)
	{
	case Game_Mode::GameMode_SinglePlayer:
		gameMode = new GameMode_SinglePlayer();
		break;
	case Game_Mode::GameMode_MultiPlayer:
		gameMode = new GameMode_MultiPlayer();
		break;
	case Game_Mode::GameMode_MapEditor:
		gameMode = new GameMode_MapEditor();
		break;
	}
	gameMode->init();


	Renderer::initFrameBuffer();

	Game::resetKeys();
	Renderer::resetFrameCount();


	gameStopWatch.start();

	SDL_SetRelativeMouseMode(SDL_TRUE);
}


/// <summary>
/// main game loop
/// </summary>
void Game::gameLoop()
{

	while (!close)
	{
		frametimeStopWatch.start();
		#ifdef DEBUG_LOOP
		Logger::log("Debug New Frame");
		#endif

		if(gameMode!=nullptr)
			gameMode->gameLoop();

		float64 frametime = frametimeStopWatch.stop();

		float32 fps_limit_current=0;
		if (gameState == GameState::GAME_ACTIVE || gameState == GameState::GAME_GAME_OVER) fps_limit_current = ConfigManager::fps_limit_ingame;
		if (gameState == GameState::GAME_MENU || gameState == GameState::GAME_PAUSED) fps_limit_current = ConfigManager::fps_limit_menu;

		if (frametime < 1000/ fps_limit_current && fps_limit_current !=0)
		{
			Game::sleep(1000 / fps_limit_current - frametime);
		}

		frametime = frametimeStopWatch.stop();

		delta = frametime;

		FPS = 1 / (delta/1000);

		fpsGraph->setValue(FPS);

	}

	gameMode->deinit();
	AudioManager::deinit();
	LuaManager::deinit();
	ConfigManager::writeAllConfigs();
}

/// <summary>
/// process the Collision for every object
/// </summary>
void Game::processCollision()
{
	for (std::shared_ptr<Object> object : objects)
	{
		if (NetworkManager::getIsConnected() && !(object->getType() & ObjectType::Object_Player)) continue;


		CollisionResult collisionResult = object->checkCollision();
		if (!collisionResult.collided) continue;
		if (!object->getEnabled()) continue;

		for (CollidedObject* collidedObject : collisionResult.collidedObjectList)
		{
			if (object->getType() & ObjectType::Object_Character && collidedObject->onTop == true)
			{
				Character* character = static_cast<Character*>(object.get());
				character->activateJumping();
			}

			if ((object->getType() & ObjectType::Object_NPC) && (collidedObject->object->getType() & (ObjectType::Object_Entity | ObjectType::Object_Character)))
			{
				NPC* npc = static_cast<NPC*>(object.get());
				npc->evade(collisionResult);
			}
		}


		object->reactToCollision(collisionResult);

	}
}

/// <summary>
/// in this methods the game decides what needs to be rendered
/// </summary>
void Game::render()
{

		switch (gameState)
		{
		case GameState::GAME_ACTIVE:
		case GameState::GAME_GAME_OVER:
		{
			if (players.size() > 0) {

				if (!Game::showMap)
				{
					Renderer::calcLight();

					if (ConfigManager::shadow_option != ShadowOption::off)
						Renderer::calcShadows();

					Renderer::frameBuffer.bind();
					Renderer::clearBuffer();
					Renderer::renderOpaqueObjects();
					Renderer::renderSkybox(glm::mat4(glm::mat3(players[0]->getView())), players[0]->getProj());
					Renderer::renderTransparentObjects();

					for (Font3D* text : Game::texts)
					{
						text->renderTexture();
					}

					Renderer::frameBuffer.unbind();
				}
				else
				{
					Renderer::frameBuffer.bind();
					Renderer::clearBuffer();
					Renderer::renderMap();
					Renderer::frameBuffer.unbind();
				}

				//Postprocessing
				Renderer::postProcessing();

				//show the Shadow map
				if (showShadowMap) Renderer::showShadowMap();


				if (showInfo)
				{
					Renderer::renderAxis(players[0]->getLookDirection(), 8, 5);
					Renderer::renderAxis(Renderer::transformedSunDirection3, 8, 3);

					UI::updateFPS();
					UI::updatePos(players[0]);
					UI::updateRot(players[0]);
				}

				UI::drawUI();

				if (Game::showMap)
				{
					Renderer::drawMapOverlay();
				}

			}
		}
		break;
		case GameState::GAME_MENU:
		{
			if (menu_current == nullptr) break;

			GLCALL(glClearColor(0, 0, 0, 0));
			GLCALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

			Renderer::postProcessing();
			GLCALL(glDisable(GL_DEPTH_TEST));
			GLCALL(glDisable(GL_CULL_FACE));
			menu_current->drawMenu();
			GLCALL(glEnable(GL_DEPTH_TEST));
			GLCALL(glEnable(GL_CULL_FACE));
		}
		break;
		}

		Game::swapBuffer();
}

/// <summary>
/// process the Input from Mouse and Keyboard
/// </summary>
void Game::processInput()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_QUIT)
		{
			close = true;
		}
		else if (event.type == SDL_KEYDOWN)
		{
			keyPressed(event.key.keysym.sym);
		}
		else if (event.type == SDL_KEYUP)
		{
			keyReleased(event.key.keysym.sym);
		}
		else if (event.type == SDL_MOUSEMOTION) {
			if (gameState == GameState::GAME_ACTIVE || gameState == GameState::GAME_GAME_OVER)
			{
				if (SDL_GetRelativeMouseMode() && players.size() > 0) {
					players[0]->onMouseMove(event.motion.xrel, event.motion.yrel);
					AudioManager::updateAudioListener();
					NetworkManager::sendPlayerRotation();
				}
			}
			else if (gameState == GameState::GAME_MENU)
			{
				menu_current->onMouseMove(event.motion.x, event.motion.y);
			}
		}
		else if (event.type == SDL_MOUSEBUTTONDOWN) {
			if (gameState == GameState::GAME_ACTIVE || gameState == GameState::GAME_GAME_OVER)
			{
				if (!SDL_GetRelativeMouseMode())
				{
					SDL_SetRelativeMouseMode(SDL_TRUE);
					continue;
				}
			}
			else if (gameState == GameState::GAME_MENU)
			{
				if (menu_current->onMouseClick(event.motion.x, event.motion.y, event.button) == 2)
				{
					continue;
				}
			}
			pressedMouseButtons[event.button.button] = true;

		}
		else if (event.type == SDL_MOUSEBUTTONUP) {
			pressedMouseButtons[event.button.button] = false;
		}
	}
	//continious Action Keys, mulitple times per pressing
	if (pressedMouseButtons[SDL_BUTTON_LEFT])
	{
		if (gameState == GameState::GAME_ACTIVE)
		{
			if (!NetworkManager::getIsConnected())
			{
				if (players.size() > 0) {
					players[0]->shoot();
				}
			}
			else
			{
				NetworkManager::sendShoot();
			}
		}
		else if (gameState == GameState::GAME_MENU)
		{
			menu_current->onMouseDown(event.motion.x, event.motion.y, event.button);
		}

	}

	if (isKeyPressed(PlayerAction::moveForward))
	{
		if (gameState == GameState::GAME_ACTIVE || gameState == GameState::GAME_GAME_OVER)
		{
			if (players.size() > 0) {
				players[0]->moveForward();
				AudioManager::updateAudioListener();
			}
		}
	}
	if (isKeyPressed(PlayerAction::moveBackward))
	{
		if (gameState == GameState::GAME_ACTIVE || gameState == GameState::GAME_GAME_OVER)
		{
			if (players.size() > 0) {
				players[0]->moveBackward();
				AudioManager::updateAudioListener();
			}
		}
	}
	if (isKeyPressed(PlayerAction::moveLeft))
	{
		if (gameState == GameState::GAME_ACTIVE || gameState == GameState::GAME_GAME_OVER)
		{
			if (players.size() > 0) {
				players[0]->moveLeft();
				AudioManager::updateAudioListener();
			}
		}
		else if (gameState == GameState::GAME_MENU)
		{
			menu_current->leftOnSelectedMenuElement();
		}
	}
	if (isKeyPressed(PlayerAction::moveRight))
	{
		if (gameState == GameState::GAME_ACTIVE || gameState == GameState::GAME_GAME_OVER)
		{
			if (players.size() > 0) {
				players[0]->moveRight();
				AudioManager::updateAudioListener();
			}
		}
		else if (gameState == GameState::GAME_MENU)
		{
			menu_current->rightOnSelectedMenuElement();
		}
	}
	if (isKeyPressed(PlayerAction::jump))
	{
		if (gameState == GameState::GAME_ACTIVE || gameState == GameState::GAME_GAME_OVER)
		{
			if (players.size() > 0) {
				players[0]->jump();
			}
		}
	}
	if (isKeyPressed(PlayerAction::sprint))
	{
		if (gameState == GameState::GAME_ACTIVE || gameState == GameState::GAME_GAME_OVER)
		{
			if (players.size() > 0) {
				players[0]->run(true);
			}
		}
	}

	if (!isKeyPressed(PlayerAction::sprint))
	{
		if (gameState == GameState::GAME_ACTIVE || gameState == GameState::GAME_GAME_OVER)
		{
			if (players.size()>0) {
				players[0]->run(false);
			}
		}
	}
	if (isKeyPressed(PlayerAction::crouch))
	{
		if (gameState == GameState::GAME_ACTIVE || gameState == GameState::GAME_GAME_OVER)
		{
			if (players.size() > 0) {
				players[0]->crouch(true);
			}
		}
	}
	if (!isKeyPressed(PlayerAction::crouch))
	{
		if (gameState == GameState::GAME_ACTIVE || gameState == GameState::GAME_GAME_OVER)
		{
			if (players.size() > 0) {
				players[0]->crouch(false);
			}
		}
	}
	if (isKeyPressed(PlayerAction::interact))
	{
		if (gameState == GameState::GAME_ACTIVE || gameState == GameState::GAME_GAME_OVER)
		{
			if (players.size() > 0) {
				std::shared_ptr<Object> objectPlayerLookingAt = players[0]->getObjectLookingAt();
				if (objectPlayerLookingAt != nullptr)
				{
					if (players[0]->getDistance(objectPlayerLookingAt) < 10)
					{
						objectPlayerLookingAt->interact_hold();

						objectPlayerLookingAt->markObject();
					}
				}
			}
		}
	}
}

/// <summary>
/// function for when a key is pressed
/// </summary>
/// <param name="key">the pressed key</param>
void Game::keyPressed(SDL_Keycode key)
{
	PlayerAction action = PlayerAction::None;

	auto it = keybindings.find(key);
	if (it != keybindings.end()) {
		action = it->second;
	}
	else { Logger::log("Keybinding not found!"); /*return;*/ }

	//Single Action Keys, just one time per pressing
	if (!Game::isKeyPressed(key))
	{
		switch (gameState)
		{
			case GameState::GAME_ACTIVE:
			case GameState::GAME_GAME_OVER:
			case GameState::GAME_PAUSED:
			{
				switch (action)
				{
					case PlayerAction::interact:
					{//players[0]->interactWithObject();
						std::shared_ptr<Object> objectPlayerLookingAt = players[0]->getObjectLookingAt();
						if (objectPlayerLookingAt != nullptr)
						{
							if (players[0]->getDistance(objectPlayerLookingAt) < 10)
							{
								//objectPlayerLookingAt->markObject();
								objectPlayerLookingAt->interact_click();
							}
						}
						break;
					}
					case PlayerAction::toggleFlashlight:
					{
						players[0]->toggleFlashlight();
						break;
					}
					case PlayerAction::toggleMap:
					{
						Game::toggleMap();
						break;
					}
				}
				break;
			}
			case GameState::GAME_MENU:
			{
				menu_current->onKeyDown(key);

				/*switch (action)
				{
					case PlayerAction::moveForward:
						menu_current->selectPreviousElement();
						AudioManager::play2D("audio/select.wav");
						break;
					case PlayerAction::moveBackward:
						menu_current->selectNextElement();
						AudioManager::play2D("audio/select.wav");
						break;
					case PlayerAction::enter:
						menu_current->enterSelectedMenuElement();
						AudioManager::play2D("audio/select.wav");
						break;
					case PlayerAction::jump:
						menu_current->enterSelectedMenuElement();
						AudioManager::play2D("audio/select.wav");
						break;
				}*/
				break;
			}
		}

		//Keybind which should work in every gamemode
		switch (action)
		{
		case PlayerAction::toggleInfo:
			showInfo = !showInfo;
			break;
		case PlayerAction::toggleWireframe:
			Renderer::toggleWireframe();
			break;
		case PlayerAction::toggleShowNormals:
			Renderer::toggleShowNormals();
			break;
		case PlayerAction::toggleShowShadowMap:
			showShadowMap = !showShadowMap;
			break;
		case PlayerAction::togglePostprocess:

			break;
		case PlayerAction::pause:
			togglePause();
			break;
		case PlayerAction::menu:
			if (toggleMenu()) break;
			if (toggleSubMenu(MenuType::MENU_OPTIONS)) break;
			break;
		case PlayerAction::toggleFullscreen:
			toggleFullscreen();
			break;
		case PlayerAction::toggleConsole:
			openConsole();
			break;
		}

	}

	Game::setKeyPressed(key, true);
}

/// <summary>
/// function for when a key is released
/// </summary>
/// <param name="key">the released key</param>
void Game::keyReleased(SDL_Keycode key)
{
	PlayerAction action = PlayerAction::None;

	auto it = keybindings.find(key);
	if (it != keybindings.end()) {
		action = it->second;
	}
	if (action == PlayerAction::None) { Logger::log("Keybinding not found!"); /*return;*/ }

	Game::setKeyPressed(key, false);
}

bool Game::isKeyPressed(SDL_Keycode key)
{
	int keynumber = (int)key;
	if (keynumber >= 1073741881) keynumber -= 1073741753;
	 return pressedKeys[keynumber];
}

bool Game::isKeyPressed(PlayerAction playeraction)
{
	SDL_Keycode key;

	for (auto it = keybindings.begin(); it != keybindings.end(); it++)
	{
		if (it->second == playeraction)
		{
			key = it->first;
			break;
		}
	}

	return isKeyPressed(key);
}

void Game::setKeyPressed(SDL_Keycode key, bool pressed)
{
	int keynumber = (int)key;
	if (keynumber >= 1073741881) keynumber -= 1073741753;
	pressedKeys[keynumber] = pressed;
}

void Game::resetKeys()
{
	for (int i = 0; i < 40; i++)
	{
		pressedKeys[i] = false;
	}
	for (int i = 0; i < 6; i++)
	{
		pressedMouseButtons[i] = false;
	}

	SDL_Event event1;
	while (SDL_PollEvent(&event1))
	{

	}
}

/// <summary>
/// deletes or disable object, that are destroyed
/// </summary>
void Game::deleteObjects()
{
	for (int i = objects.size() - 1; i >= 0; i--)
	{
		if (objects[i]->getHealth() <= 0)
		{
			objects[i]->disable();
			
			//objects.erase(objects.begin() + i);
			//i++;
			/*std::shared_ptr<Object> objectToDelete = objects[i];

			int index = objectToDelete->getNumber();

			if (objectToDelete->getType() & ObjectType::Object_NPC)
			{
				auto it = std::find(npcs.begin(), npcs.end(), objectToDelete);
				if (it != npcs.end()) { npcs.erase(it); }
			}

			if (objectToDelete->getType() & ObjectType::Object_Character)
			{
				auto it2 = std::find(characters.begin(), characters.end(), objectToDelete);
				if (it2 != characters.end()) { characters.erase(it2); }
			}

			if (objectToDelete->getType() & ObjectType::Object_Bullet)
			{
				auto it3 = std::find(bullets.begin(), bullets.end(), objectToDelete);
				if (it3 != bullets.end()) { bullets.erase(it3); }
			}

			auto it1 = std::find(objects.begin(), objects.end(), objectToDelete);
			if (it1 != objects.end()) { objects.erase(it1); }
			*/
		}
	}

	if (bullets.size() > ConfigManager::max_bullets)
	{
		std::shared_ptr<Object> objectToDelete = bullets[0];

		auto it3 = std::find(bullets.begin(), bullets.end(), objectToDelete);
		if (it3 != bullets.end()) { bullets.erase(it3); }

		auto it1 = std::find(objects.begin(), objects.end(), objectToDelete);
		if (it1 != objects.end()) { objects.erase(it1); }
	}
}

void Game::togglePause()
{
	Game::resetKeys();

	if (gameState == GameState::GAME_PAUSED)
	{
		gameState = GameState::GAME_ACTIVE;
	}
	else if (gameState == GameState::GAME_ACTIVE)
	{
		gameState = GameState::GAME_PAUSED;
		UI::drawPause();
		Game::swapBuffer();
	}
}

bool Game::toggleMenu()
{
	Game::resetKeys();

	if (gameState == GameState::GAME_PAUSED || gameState == GameState::GAME_ACTIVE || gameState == GameState::GAME_GAME_OVER)
	{
		gameState = GameState::GAME_MENU;
		menu_current = menu_Pause;
		SDL_SetRelativeMouseMode(SDL_FALSE);
		return true;
	}
	else if (gameState == GameState::GAME_MENU)
	{
		if (players.size() > 0)
		{
			if (players[0]->getEnabled())
				gameState = GameState::GAME_ACTIVE;
			else
				gameState = GameState::GAME_GAME_OVER;
			SDL_SetRelativeMouseMode(SDL_TRUE);
			return true;
		}
		else
		{
			Game::quit();
		}
	}
	return false;
}

bool Game::toggleSubMenu(MenuType submenu)
{
	if (gameState != GameState::GAME_MENU) return false;

	Game::resetKeys();

	Menu* newMenu = menu_last;


	switch (submenu)
	{
	case MenuType::MENU_MAIN:
		newMenu = menu_Main;
		break;
	case MenuType::MENU_OPTIONS:
		newMenu = menu_Options;
		break;
	case MenuType::MENU_PAUSE:
		newMenu = menu_Pause;
		break;
	case MenuType::MENU_MULTIPLAYER:
		newMenu = menu_Multiplayer;
		break;
	case MenuType::MENU_MAPEDITOR:
		newMenu = menu_MapEditor;
		break;
	}

	if (menu_current == menu_Options)
	{
		ConfigManager::writeAllConfigs();
	}
	
	if (newMenu != menu_current)
	{
		menu_last = menu_current;
		menu_current = newMenu;
		return true;
	} 
	else
	{
		menu_current = menu_last;
		return true;
	}

	return false;

}

void Game::toggleFullscreen()
{
	Uint32 FullscreenFlag = SDL_WINDOW_FULLSCREEN;
	bool IsFullscreen = SDL_GetWindowFlags(window) & FullscreenFlag;
	SDL_SetWindowFullscreen(window, IsFullscreen ? 0 : FullscreenFlag);

	if (IsFullscreen) //now windowed
		Game::toggleFullscreen(FullscreenOption::windowed);
	else
		Game::toggleFullscreen(FullscreenOption::fullscreen);
}

void Game::toggleFullscreen(FullscreenOption option)
{
	ConfigManager::fullscreen_option = option;
	int resolutionW, resolutionH;

	switch (option)
	{
	case FullscreenOption::fullscreen:
		SDL_SetWindowFullscreen(window, true);

		resolutionW = ConfigManager::windowed_resolution_width;
		resolutionH = ConfigManager::windowed_resolution_height;
		break;

	case FullscreenOption::windowed:
	case FullscreenOption::windowed_borderless:
		SDL_SetWindowFullscreen(window, false);

		resolutionW = ConfigManager::fullscreen_resolution_width;
		resolutionH = ConfigManager::fullscreen_resolution_height;
		break;
	}

	Game::changeSize(resolutionW, resolutionH);
}

int Game::getWindowWidth()
{
	int width, height;
	SDL_GetWindowSize(window, &width, &height);

	return width;
}

int Game::getWindowHeight()
{
	int width, height;
	SDL_GetWindowSize(window, &width, &height);

	return height;
}

void Game::openConsole()
{
	std::string enteredText = "";

	std::getline(std::cin, enteredText);

	std::vector<std::string> params;
	Helper::split(enteredText, params, ' ');

	if (params[0] == "x")
	{
		players[0]->setPosition(glm::vec3(std::stof(params[1]), players[0]->getPosition().y, players[0]->getPosition().z));
	}
	else if (params[0] == "y")
	{
		players[0]->setPosition(glm::vec3(players[0]->getPosition().x, std::stof(params[1]), players[0]->getPosition().z));
	}
	else if (params[0] == "z")
	{
		players[0]->setPosition(glm::vec3(players[0]->getPosition().x, players[0]->getPosition().y, std::stof(params[1])));
	}
	else if (params[0] == "xyz")
	{
		players[0]->setPosition(glm::vec3(std::stof(params[1]), std::stof(params[2]), std::stof(params[3])));
	}
	else if (params[0] == "map")
	{
		std::string message = params[1];
		Helper::eraseSubStr(message, "^");
		NetworkManager::sendData(NetworkCommand::change_map, message);
	}

}

void Game::changeSize(int w, int h)
{
	SDL_SetWindowSize(window, w, h);
	Renderer::initFrameBuffer();
}

void Game::sleep(int ms)
{
	std::chrono::duration<double, std::milli> delta_ms(ms);
	auto delta_ms_duration = std::chrono::duration_cast<std::chrono::milliseconds>(delta_ms);
	std::this_thread::sleep_for(std::chrono::milliseconds(delta_ms_duration));
}