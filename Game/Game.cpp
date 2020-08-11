#include "Game.h"



#define STB_IMAGE_IMPLEMENTATION
#include "libs/stb_image.h"
#undef STB_IMAGE_IMPLEMENTATION


SDL_Window* Game::window;

std::vector<std::shared_ptr<Object>> Game::map;
std::vector<std::shared_ptr<Object>> Game::objects;
std::vector< std::shared_ptr<Character> > Game::characters;
std::vector< std::shared_ptr<Player> > Game::players;
std::vector< std::shared_ptr<NPC> > Game::npcs;
std::vector< std::shared_ptr<Bullet> > Game::bullets;


bool Game::pressedKeys[40];
bool Game::pressedMouseButtons[6];

float32  Game::FPS = 0;
bool  Game::close = false;



float32 Game::delta = 0;

bool Game::showInfo = false;
GameState Game::gameState = GameState::GAME_MENU;
GameState Game::newGameState;

bool Game::showShadowMap = false;
bool Game::showMap = false;

Menu* Game::menu_Main = nullptr;
Menu* Game::menu_Pause = nullptr;
Menu* Game::menu_Options = nullptr;

Menu* Game::menu_current = menu_Main;
Menu* Game::menu_last = menu_Main;

irrklang::ISoundEngine* Game::SoundEngine = irrklang::createIrrKlangDevice();

UI_Element_Graph* Game::fpsGraph;
UI_Element_Label* Game::lbl_stopwatch1, * Game::lbl_stopwatch2, * Game::lbl_stopwatch3, * Game::lbl_stopwatch4, * Game::lbl_stopwatch5;
StopWatch Game::stopwatch1;
StopWatch Game::gameStopWatch;


/// <summary>
/// this methods starts the game
/// </summary>
void Game::startGame()
{
	delta = 0.0f;
	gameState = GameState::GAME_ACTIVE;

	Renderer::initFrameBuffer();

	Game::resetKeys();
	Renderer::resetFrameCount();


	gameStopWatch.start();

	SDL_SetRelativeMouseMode(SDL_TRUE);
}



/// <summary>
/// this function inits everything before the game can be started
/// </summary>
void Game::init()
{

	Renderer::initOpenGL();
	Renderer::loadShader();

	NetworkManager::init();
	NetworkManager::deinit();

	LuaManager::testLua();

	UI::init();

	fpsGraph = new UI_Element_Graph(10, getWindowHeight() * 3 / 4, 100, 100, 0, glm::vec4(0, 0, 1, 1), glm::vec4(0.2, 0.2, 0.2, 0.4), true);
	UI::addElement(fpsGraph);

	lbl_stopwatch1 = new UI_Element_Label(10, 50, 100, 50, "", 0, 1, glm::vec4(1, 1, 1, 1), glm::vec4(0, 0, 0, 0.2), true);
	UI::addElement(lbl_stopwatch1);

	lbl_stopwatch2 = new UI_Element_Label(10, 70, 100, 50, "", 0, 1, glm::vec4(1, 1, 1, 1), glm::vec4(0, 0, 0, 0.2), true);
	UI::addElement(lbl_stopwatch2);

	lbl_stopwatch3 = new UI_Element_Label(10, 90, 100, 50, "", 0, 1, glm::vec4(1, 1, 1, 1), glm::vec4(0, 0, 0, 0.2), true);
	UI::addElement(lbl_stopwatch3);

	lbl_stopwatch4 = new UI_Element_Label(10, 110, 100, 50, "", 0, 1, glm::vec4(1, 1, 1, 1), glm::vec4(0, 0, 0, 0.2), true);
	UI::addElement(lbl_stopwatch4);

	lbl_stopwatch5 = new UI_Element_Label(10, 130, 100, 50, "", 0, 1, glm::vec4(1, 1, 1, 1), glm::vec4(0, 0, 0, 0.2), true);
	UI::addElement(lbl_stopwatch5);


	Renderer::drawLoadingScreen();

	//std::vector<std::string> maps =	ResourceManager::readAllMaps();

	//std::string levelname = ConfigManager::level;
	//Map::load("level_farm");

	menu_Main = new Menu_Main();
	menu_Pause = new Menu_Pause();
	menu_Options = new Menu_Options();

	menu_current = menu_Main;
	gameState = GameState::GAME_MENU;

	Renderer::init();

	SoundEngine->setSoundVolume(ConfigManager::music_volume);

	irrklang::ISound* music = SoundEngine->play2D("audio/breakout.mp3", true);


	gameLoop();
}

/// <summary>
/// main game loop
/// </summary>
void Game::gameLoop()
{
	std::chrono::system_clock::time_point a = std::chrono::system_clock::now();
	std::chrono::system_clock::time_point b = std::chrono::system_clock::now();



	while (!close)
	{

		a = std::chrono::system_clock::now();

		#pragma region gameloop

		stopwatch1.start();
		processInput();
		double stopwatch1duration = stopwatch1.stop();
		lbl_stopwatch1->setText("Input: " + std::to_string(stopwatch1duration));

		if (gameState == GameState::GAME_ACTIVE || gameState == GameState::GAME_GAME_OVER)
		{
			GLCALL(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
			GLCALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

			for (std::shared_ptr<Object> object : objects)
			{
				if (!object->getEnabled()) continue;
				object->calculationBeforeFrame();
			}


			stopwatch1.start();
			for (std::shared_ptr<NPC> npc : npcs)
			{
				if (!npc->getEnabled()) continue;
				npc->doCurrentTask();
			}
			double stopwatch2duration = stopwatch1.stop();
			lbl_stopwatch2->setText("NPCs: " + std::to_string(stopwatch2duration));

			//Move every Object
			for (std::shared_ptr<Object> object : objects)
			{
				if (!object->getEnabled()) continue;
				if (object->getType() == ObjectType::Object_Environment) continue; //Environment doesnt move

				object->fall();
				object->move();

			}

			stopwatch1.start();
			for (std::shared_ptr<Bullet> bullet : bullets)
			{
				if (!bullet->getEnabled()) continue;
				bullet->checkHit();
			}

			//Check every Object for collision
			processCollision();
			double stopwatch3duration = stopwatch1.stop();
			lbl_stopwatch3->setText("Col: " + std::to_string(stopwatch3duration));

			for (std::shared_ptr<Player> player : players)
			{
				if (!player->getEnabled()) continue;
				player->updateCameraPosition();
				//player->update();
				if (player->getPosition().x > 95 && player->getPosition().z > 95)
				{
					UI_Element* eastereggLabel = new UI_Element_Label(getWindowWidth() / 2 - 200, getWindowHeight() / 2 - 100, 10, 10, "Nice, du hast das Easter-Egg gefunden", 1000, 1, glm::vec4(1, 0, 0, 1), glm::vec4(0.2, 0.2, 0.2, 0.4), false);
					UI::addElement(eastereggLabel);
				}
			}

			for (std::shared_ptr<Character> character : characters)
			{
				if (!character->getEnabled()) continue;
				character->resetVerticalMovement();
			}

			deleteObjects();

			if (npcs.size() <= 0 && gameState == GameState::GAME_ACTIVE) //todo npcs get disabled
			{
				UI_Element* victoryLabel = new UI_Element_Label(getWindowWidth() / 2 - 100, getWindowHeight() / 2, 10, 10, "Du hast alle Bots besiegt", 1000, 1, glm::vec4(0, 0, 1, 1), glm::vec4(0.2, 0.2, 0.2, 0.4), false);
				UI::addElement(victoryLabel);
				gameState = GameState::GAME_GAME_OVER;
			}

			stopwatch1.start();
			//testing - Raypicking
			std::shared_ptr<Object> objectPlayerLookingAt = players[0]->getObjectLookingAt();
			if (objectPlayerLookingAt != nullptr)
			{
				if (players[0]->getDistance(objectPlayerLookingAt) < 10)
				{
					objectPlayerLookingAt->markObject();
				}
			}
			double stopwatch5duration = stopwatch1.stop();
			lbl_stopwatch5->setText("Raypicking: " + std::to_string(stopwatch5duration));

		}
		
		stopwatch1.start();
		if (gameState != GameState::GAME_PAUSED)
		{
			render();
		}
		double stopwatch4duration = stopwatch1.stop();
		lbl_stopwatch4->setText("Render: " + std::to_string(stopwatch4duration));

		if (gameState == GameState::GAME_ACTIVE || gameState == GameState::GAME_GAME_OVER)
		{
			for (std::shared_ptr<Object> object : objects)
			{
				if (!object->getEnabled()) continue;
				object->calculationAfterFrame();
			}
		}

		#pragma endregion

		b = std::chrono::system_clock::now();

		std::chrono::duration<double, std::milli> work_time = b - a;

		float32 fps_limit_current=0;
		if (gameState == GameState::GAME_ACTIVE || gameState == GameState::GAME_GAME_OVER) fps_limit_current = ConfigManager::fps_limit_ingame;
		if (gameState == GameState::GAME_MENU || gameState == GameState::GAME_PAUSED) fps_limit_current = ConfigManager::fps_limit_menu;

		if (work_time.count() < 1000/ fps_limit_current && fps_limit_current !=0)
		{
			std::chrono::duration<double, std::milli> delta_ms(1000 / fps_limit_current - work_time.count());
			auto delta_ms_duration = std::chrono::duration_cast<std::chrono::milliseconds>(delta_ms);
			std::this_thread::sleep_for(std::chrono::milliseconds(delta_ms_duration.count()));
		}

		b = std::chrono::system_clock::now();
		std::chrono::duration<double, std::milli> loop_time = b - a;

		delta = loop_time.count();
		FPS = 1 / (delta/1000);

		fpsGraph->setValue(FPS);

	}

	ConfigManager::writeAllConfigs();
}

/// <summary>
/// process the Collision for every object
/// </summary>
void Game::processCollision()
{
	for (std::shared_ptr<Object> object : objects)
	{
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
				if (SDL_GetRelativeMouseMode()) {
					players[0]->onMouseMove(event.motion.xrel, event.motion.yrel);
					updateAudioListener();
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

	if (pressedMouseButtons[SDL_BUTTON_LEFT])
	{
		if (gameState == GameState::GAME_ACTIVE)
		{
			if (players[0]->shoot())	SoundEngine->play2D("audio/shoot.wav", false);
		}
		else if (gameState == GameState::GAME_MENU)
		{
			menu_current->onMouseDown(event.motion.x, event.motion.y, event.button);
		}

	}

	if (pressedKeys[PlayerAction::moveForward])
	{
		if (gameState == GameState::GAME_ACTIVE || gameState == GameState::GAME_GAME_OVER)
		{
			players[0]->moveForward();
			updateAudioListener();
		}
	}
	if (pressedKeys[PlayerAction::moveBackward])
	{
		if (gameState == GameState::GAME_ACTIVE || gameState == GameState::GAME_GAME_OVER)
		{
			players[0]->moveBackward();
			updateAudioListener();
		}
	}
	if (pressedKeys[PlayerAction::moveLeft])
	{
		if (gameState == GameState::GAME_ACTIVE || gameState == GameState::GAME_GAME_OVER)
		{
			players[0]->moveLeft();
			updateAudioListener();
		}
		else if (gameState == GameState::GAME_MENU)
		{
			menu_current->leftOnSelectedMenuElement();
		}
	}
	if (pressedKeys[PlayerAction::moveRight])
	{
		if (gameState == GameState::GAME_ACTIVE || gameState == GameState::GAME_GAME_OVER)
		{
			players[0]->moveRight();
			updateAudioListener();
		}
		else if (gameState == GameState::GAME_MENU)
		{
			menu_current->rightOnSelectedMenuElement();
		}
	}
	if (pressedKeys[PlayerAction::jump])
	{
		if (gameState == GameState::GAME_ACTIVE || gameState == GameState::GAME_GAME_OVER)
		{
			players[0]->jump();
		}
	}
	if (pressedKeys[PlayerAction::sprint])
	{
		if (gameState == GameState::GAME_ACTIVE || gameState == GameState::GAME_GAME_OVER)
		{
			players[0]->run(true);
		}
	}
	if (!pressedKeys[PlayerAction::sprint])
	{
		if (gameState == GameState::GAME_ACTIVE || gameState == GameState::GAME_GAME_OVER)
		{
			players[0]->run(false);
		}
	}
	if (pressedKeys[PlayerAction::crouch])
	{
		if (gameState == GameState::GAME_ACTIVE || gameState == GameState::GAME_GAME_OVER)
		{
			players[0]->crouch(true);
		}
	}
	if (!pressedKeys[PlayerAction::crouch])
	{
		if (gameState == GameState::GAME_ACTIVE || gameState == GameState::GAME_GAME_OVER)
		{
			players[0]->crouch(false);
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
	if (action == PlayerAction::None) { Logger::log("Keybinding not found!"); return; }

	//Single Action Keys, just one time per pressing
	if (!pressedKeys[action])
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
						players[0]->interactWithObject();
						break;
					case PlayerAction::toggleFlashlight:
						players[0]->toggleFlashlight();
						break;
					case PlayerAction::toggleMap:
						Game::toggleMap();
						break;
				}
				break;
			}
			case GameState::GAME_MENU:
			{
				switch (action)
				{
					case PlayerAction::moveForward:
						menu_current->selectPreviousElement();
						SoundEngine->play2D("audio/select.wav", false);
						break;
					case PlayerAction::moveBackward:
						menu_current->selectNextElement();
						SoundEngine->play2D("audio/select.wav", false);
						break;
					case PlayerAction::enter:
						menu_current->enterSelectedMenuElement();
						SoundEngine->play2D("audio/select.wav", false);
						break;
					case PlayerAction::jump:
						menu_current->enterSelectedMenuElement();
						SoundEngine->play2D("audio/select.wav", false);
						break;
				}
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
		case toggleShowShadowMap:
			showShadowMap = !showShadowMap;
			break;
		case togglePostprocess:

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

	pressedKeys[action] = true;
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
	if (action == PlayerAction::None) { Logger::log("Keybinding not found!"); return; }


	pressedKeys[action] = false;
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
		if(players[0]->getEnabled())
			gameState = GameState::GAME_ACTIVE;
		else
			gameState = GameState::GAME_GAME_OVER;
		SDL_SetRelativeMouseMode(SDL_TRUE);
		return true;
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

void Game::updateAudioListener()
{
	irrklang::vec3df Lposition = irrklang::vec3df(players[0]->getPosition().x, players[0]->getPosition().y, players[0]->getPosition().z);
	irrklang::vec3df LlookAt = irrklang::vec3df(-players[0]->getLookDirection().x, players[0]->getLookDirection().y, -players[0]->getLookDirection().z);
	Game::SoundEngine->setListenerPosition(Lposition, LlookAt);
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

	std::cin >> enteredText;

	if (enteredText[0] == 'x')
	{
		players[0]->setPosition(glm::vec3(std::stof(enteredText.substr(1)), players[0]->getPosition().y, players[0]->getPosition().z));
	}
	if (enteredText[0] == 'y')
	{
		players[0]->setPosition(glm::vec3(players[0]->getPosition().x, std::stof(enteredText.substr(1)), players[0]->getPosition().z));
	}
	if (enteredText[0] == 'z')
	{
		players[0]->setPosition(glm::vec3(players[0]->getPosition().x, players[0]->getPosition().y, std::stof(enteredText.substr(1))));
	}
}

void Game::changeSize(int w, int h)
{
	SDL_SetWindowSize(window, w, h);
	Renderer::initFrameBuffer();
}