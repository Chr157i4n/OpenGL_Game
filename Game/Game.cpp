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


bool Game::pressedKeys[20];

float32  Game::FPS = 0;
float32  Game::fps_limit = 0;
bool  Game::close = false;

uint64 Game::perfCounterFrequency;
uint64 Game::lastCounter;
float32 Game::delta;

bool Game::showInfo = false;
GameState Game::gameState;
int Game::maxBulletCount = 20;

bool Game::showShadowMap = false;
bool Game::postprocess = true;

Menu* Game::menu = nullptr;

irrklang::ISoundEngine* Game::SoundEngine = irrklang::createIrrKlangDevice();

void Game::startGame()
{
	perfCounterFrequency = SDL_GetPerformanceFrequency();
	lastCounter = SDL_GetPerformanceCounter();
	delta = 0.0f;
	gameState = GameState::GAME_ACTIVE;

	SDL_SetRelativeMouseMode(SDL_TRUE);

	float musicvolume = std::stof(ConfigManager::readConfig("musicvolume"));
	SoundEngine->setSoundVolume(musicvolume / 100);

	irrklang::ISound* music = SoundEngine->play2D("audio/breakout.mp3", true);



	gameLoop();
}


void Game::init()
{
	Renderer::initOpenGL();
	Renderer::loadShader();

	UI::init();

	Renderer::drawLoadingScreen();


	maxBulletCount = stoi(ConfigManager::readConfig("max_bullets"));
	fps_limit = stof(ConfigManager::readConfig("fps_limit"));
	std::string levelname = ConfigManager::readConfig("level");
	Map::load(levelname);

	menu = new Menu();

	Renderer::init();
}

void Game::gameLoop()
{
	std::chrono::system_clock::time_point a = std::chrono::system_clock::now();
	std::chrono::system_clock::time_point b = std::chrono::system_clock::now();

	UI_Element_Graph* fpsGraph = new UI_Element_Graph(10, getWindowHeight() * 3 / 4, 100, 100, 0, glm::vec4(0, 0, 1, 1), true);
	UI::addElement(fpsGraph);


	while (!close)
	{

		a = std::chrono::system_clock::now();

		#pragma region gameloop

		processInput();

		if (gameState == GameState::GAME_ACTIVE || gameState == GameState::GAME_GAME_OVER)
		{
			GLCALL(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
			GLCALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

			for (std::shared_ptr<Object> object : objects)
			{
				object->calculationBeforeFrame();
			}



			for (std::shared_ptr<NPC> npc : npcs)
			{
				npc->doCurrentTask();
			}

			//Move every Object
			for (std::shared_ptr<Object> object : objects)
			{
				if (object->getType() == ObjectType::Object_Environment) continue; //Environment doesnt move

				object->fall();
				object->move();

			}

			for (std::shared_ptr<Bullet> bullet : bullets)
			{
				bullet->checkHit();
			}

			//Check every Object for collision
			processCollision();

			for (std::shared_ptr<Player> player : players)
			{
				player->updateCameraPosition();
				//player->update();
				if (player->getPosition().x > 95 && player->getPosition().z > 95)
				{
					UI_Element* eastereggLabel = new UI_Element_Label(getWindowWidth() / 2 - 200, getWindowHeight() / 2 - 100, "Nice, du hast das Easter-Egg gefunden", 1, 1, glm::vec4(1, 0, 0, 1), false);
					UI::addElement(eastereggLabel);
				}
			}

			for (std::shared_ptr<Character> character : characters)
			{
				character->resetVerticalMovement();
			}

			deleteObjects();

			if (npcs.size() <= 0 && gameState == GameState::GAME_ACTIVE)
			{
				UI_Element* victoryLabel = new UI_Element_Label(getWindowWidth() / 2 - 100, getWindowHeight() / 2, "Du hast alle Bots besiegt", 1, 1, glm::vec4(0, 0, 1, 1), false);
				UI::addElement(victoryLabel);
				gameState = GameState::GAME_GAME_OVER;
			}

			render();

			for (std::shared_ptr<Object> object : objects)
			{
				object->calculationAfterFrame();
			}

		}

		if (gameState == GameState::GAME_MENU)
		{
			if (menu != nullptr)
			{

				GLCALL(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
				GLCALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

				menu->drawMenu();

				SDL_GL_SwapWindow(window);
			}
		}

		#pragma endregion

		b = std::chrono::system_clock::now();

		std::chrono::duration<double, std::milli> work_time = b - a;

		if (work_time.count() < 1000/fps_limit && fps_limit!=0)
		{
			std::chrono::duration<double, std::milli> delta_ms(1000 / fps_limit - work_time.count());
			auto delta_ms_duration = std::chrono::duration_cast<std::chrono::milliseconds>(delta_ms);
			std::this_thread::sleep_for(std::chrono::milliseconds(delta_ms_duration.count()));
		}

		b = std::chrono::system_clock::now();
		std::chrono::duration<double, std::milli> loop_time = b - a;

		delta = loop_time.count();
		FPS = 1 / (delta/1000);

		fpsGraph->setValue(FPS);

	}
}

void Game::processCollision()
{
	for (std::shared_ptr<Object> object : objects)
	{
		CollisionResult collisionResult = object->checkCollision();
		if (!collisionResult.collided) continue;

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

	Renderer::calcLight();

	Renderer::calcShadows();


	if (postprocess) Renderer::frameBuffer.bind();
	Renderer::clearBuffer();
	Renderer::renderOpaqueObjects();

	Renderer::renderSkybox(glm::mat4(glm::mat3(players[0]->getView())),players[0]->getProj());
	Renderer::renderTransparentObjects();
	if (postprocess) Renderer::frameBuffer.unbind();

	//Postprocessing
	if (postprocess) Renderer::postProcessing();

	//show the Shadow map
	if (showShadowMap) Renderer::showShadowMap();


	if (showInfo)
	{
		Renderer::renderAxis(players[0]->getLookDirection(), 8, 5);
		Renderer::renderAxis(Renderer::transformedSunDirection3, 8, 3);

		UI::drawFPS();
		UI::drawPos(players[0]);
		UI::drawRot(players[0]);
	}

	UI::drawUI();

	SDL_GL_SwapWindow(window);
}

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
			if (SDL_GetRelativeMouseMode()) {
				players[0]->onMouseMove(event.motion.xrel, event.motion.yrel);
				updateAudioListener();
			}
		}
		else if (event.type == SDL_MOUSEBUTTONDOWN) {
			if (event.button.button == SDL_BUTTON_LEFT) {
				if (SDL_GetRelativeMouseMode()) {
					std::shared_ptr<Bullet> newBullet = players[0]->shoot();
					//objects.push_back(newBullet);
					//bullets.push_back(newBullet);
					if (newBullet != nullptr)	SoundEngine->play2D("audio/shoot.wav", false);
				}

				SDL_SetRelativeMouseMode(SDL_TRUE);
			}
		}
	}

	if (pressedKeys[PlayerAction::moveForward])
	{
		players[0]->moveForward();
		updateAudioListener();
	}
	if (pressedKeys[PlayerAction::moveBackward])
	{
		players[0]->moveBackward();
		updateAudioListener();
	}
	if (pressedKeys[PlayerAction::moveLeft])
	{
		players[0]->moveLeft();
		updateAudioListener();
	}
	if (pressedKeys[PlayerAction::moveRight])
	{
		players[0]->moveRight();
		updateAudioListener();
	}
	if (pressedKeys[PlayerAction::jump])
	{
		players[0]->jump();
	}
	if (pressedKeys[PlayerAction::sprint])
	{
		players[0]->run(true);
	}
	if (!pressedKeys[PlayerAction::sprint])
	{
		players[0]->run(false);
	}
	if (pressedKeys[PlayerAction::crouch])
	{
		players[0]->crouch(true);
	}
	if (!pressedKeys[PlayerAction::crouch])
	{
		players[0]->crouch(false);
	}
}

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
						// todo
						break;
					case PlayerAction::toggleFlashlight:
						players[0]->toggleFlashlight();
						break;
				}
				break;
			}
			case GameState::GAME_MENU:
			{
				switch (action)
				{
					case PlayerAction::moveForward:
						menu->selectPreviousItem();
						SoundEngine->play2D("audio/select.wav", false);
						break;
					case PlayerAction::moveBackward:
						menu->selectNextItem();
						SoundEngine->play2D("audio/select.wav", false);
						break;
					case PlayerAction::enter:
						menu->enterSelectedMenuItem();
						SoundEngine->play2D("audio/select.wav", false);
						break;
					case PlayerAction::jump:
						menu->enterSelectedMenuItem();
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
			postprocess = !postprocess;
			break;
		case PlayerAction::pause:
			togglePause();
			break;
		case PlayerAction::menu:
			toggleMenu();
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

void Game::deleteObjects()
{
	for (int i = objects.size() - 1; i >= 0; i--)
	{
		if (objects[i]->getHealth() <= 0)
		{
			//objects.erase(objects.begin() + i);
			//i++;
			std::shared_ptr<Object> objectToDelete = objects[i];

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

		}
	}

	if (bullets.size() > maxBulletCount)
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
	if (gameState == GameState::GAME_PAUSED)
	{
		gameState = GameState::GAME_ACTIVE;
	}
	else if (gameState == GameState::GAME_ACTIVE)
	{
		gameState = GameState::GAME_PAUSED;
		UI::drawPause();
		SDL_GL_SwapWindow(window);
	}
}

void Game::toggleMenu()
{
	if (gameState == GameState::GAME_PAUSED || gameState == GameState::GAME_ACTIVE || gameState == GameState::GAME_GAME_OVER)
	{
		gameState = GameState::GAME_MENU;
		SDL_SetRelativeMouseMode(SDL_FALSE);
	}
	else if (gameState == GameState::GAME_MENU)
	{
		gameState = GameState::GAME_ACTIVE;
		SDL_SetRelativeMouseMode(SDL_TRUE);
	}
}

void Game::toggleFullscreen()
{
	Uint32 FullscreenFlag = SDL_WINDOW_FULLSCREEN;
	bool IsFullscreen = SDL_GetWindowFlags(window) & FullscreenFlag;
	SDL_SetWindowFullscreen(window, IsFullscreen ? 0 : FullscreenFlag);

	if (IsFullscreen) //now windowed
	{
		int resolutionW = std::stoi(ConfigManager::readConfig("windowed_resolution_width"));
		int resolutionH = std::stoi(ConfigManager::readConfig("windowed_resolution_height"));

		SDL_SetWindowSize(window, resolutionW, resolutionH);
		Renderer::initFrameBuffer();
	}
	else {
		int resolutionW = std::stoi(ConfigManager::readConfig("fullscreen_resolution_width"));
		int resolutionH = std::stoi(ConfigManager::readConfig("fullscreen_resolution_height"));

		SDL_SetWindowSize(window, resolutionW, resolutionH);
		Renderer::initFrameBuffer();
	}
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