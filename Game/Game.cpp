#include "Game.h"

//#include <chrono>
//#include <thread>



#define STB_IMAGE_IMPLEMENTATION
#include "libs/stb_image.h"
#undef STB_IMAGE_IMPLEMENTATION


SDL_Window* Game::window;

std::vector<std::shared_ptr<Object>> Game::objects;
std::vector< std::shared_ptr<Character> > Game::characters;
std::vector< std::shared_ptr<Player> > Game::players;
std::vector< std::shared_ptr<NPC> > Game::npcs;
std::vector< std::shared_ptr<Bullet> > Game::bullets;


bool Game::pressedKeys[20];

float Game::time = 0.0f;
uint32  Game::FPS = 0;
bool  Game::close = false;

uint64 Game::perfCounterFrequency;
uint64 Game::lastCounter;
float32 Game::delta;

bool Game::showInfo = false;
GameState Game::gameState;
int Game::maxBulletCount = 20;

bool Game::showShadowMap = false;
bool Game::postprocess = true;

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
	Renderer::initOpenGL(&window);
	Renderer::loadShader();

	UI::init(window);

	Renderer::showLoadingScreen();


	maxBulletCount = stoi(ConfigManager::readConfig("max_bullets"));
	std::string levelname = ConfigManager::readConfig("level");
	Map::load(levelname, &objects, &characters, &players, &npcs);

	Renderer::init(players[0]);
}

void Game::gameLoop()
{

	UI_Element_Graph* fpsGraph = new UI_Element_Graph(10, UI::getHeight() * 3 / 4, 100, 100, 0, glm::vec4(0, 0, 1, 1), true);
	UI::addElement(fpsGraph);

	while (!close)
	{
		time += delta;

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
				//npc->followCharacter(delta / 1000, objects, players[0]);
				//npc->followNavPoints(delta / 1000, objects);
				npc->doCurrentTask(delta / 1000, objects, characters);
			}

			//Move every Object
			for (std::shared_ptr<Object> object : objects)
			{
				if (object->getType() == ObjectType::Object_Environment) continue; //Environment doesnt move

				object->move(delta / 1000, objects[1]);

				object->fall(delta / 1000);

			}

			for (std::shared_ptr<Bullet> bullet : bullets)
			{
				//bullet->fall(delta / 1000);
				//bullet->move(delta / 1000);
				bullet->checkHit(objects);
			}

			//Check every Object for collision
			processCollision();

			for (std::shared_ptr<Player> player : players)
			{
				player->updateCameraPosition();
				//player->update();
				if (player->getPosition().x > 95 && player->getPosition().z > 95)
				{
					UI_Element* eastereggLabel = new UI_Element_Label(UI::getWidth() / 2-200, UI::getHeight() / 2-100, "Nice, du hast das Easter-Egg gefunden",1, 1, glm::vec4(1, 0, 0, 1), false);
					UI::addElement(eastereggLabel);
				}
			}

			for (std::shared_ptr<Character> character : characters)
			{
				character->resetVerticalMovement();
			}

			deleteObjects();

			if (npcs.size() <= 0 && gameState== GameState::GAME_ACTIVE)
			{
				UI_Element* victoryLabel = new UI_Element_Label(UI::getWidth() / 2-100, UI::getHeight() / 2, "Du hast alle Bots besiegt", 1, 1, glm::vec4(0, 0, 1, 1), false);
				UI::addElement(victoryLabel);
				gameState = GameState::GAME_GAME_OVER;
			}

			render();

			for (std::shared_ptr<Object> object : objects)
			{
				object->calculationAfterFrame();
			}

			//std::chrono::duration<int, std::milli> timespan(100);
			//std::this_thread::sleep_for(timespan);

			
		}

		if (gameState == GameState::GAME_MENU)
		{
			GLCALL(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
			GLCALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

			UI::drawMenu();

			SDL_GL_SwapWindow(window);
		}

		uint64 endCounter = SDL_GetPerformanceCounter();
		uint64 counterElasped = endCounter - lastCounter;
		delta = ((float32)counterElasped) / (float32)perfCounterFrequency;
		FPS = (uint32) std::round((float32)perfCounterFrequency / (float32)counterElasped);

		fpsGraph->setValue((float32)perfCounterFrequency / (float32)counterElasped); //not rounded FPS

		//Logger::log("FPS: " + std::to_string(FPS));
		lastCounter = endCounter;

	}
}

void Game::processCollision()
{
	for (std::shared_ptr<Object> object : objects)
	{
		CollisionResult collisionResult = object->checkCollision(objects);
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
				//Logger::log("test");
				NPC* npc = static_cast<NPC*>(object.get());
				npc->evade(delta / 1000, collisionResult, objects);
			}
		}


		object->reactToCollision(collisionResult);

	}
}

void Game::render()
{

	Renderer::calcLight(players[0]);

	Renderer::calcShadows(objects);


	if (postprocess) Renderer::frameBuffer.bind();
	Renderer::clearBuffer();
	Renderer::renderSkybox(players[0]);
	Renderer::renderObjects(players[0], objects);
	if (postprocess) Renderer::frameBuffer.unbind();

	//Postprocessing
	if (postprocess) Renderer::postProcessing();

	if (showShadowMap) Renderer::showShadowMap();


	if (showInfo)
	{
		Renderer::renderAxis(players[0]->getLookDirection(), 8, 5);
		Renderer::renderAxis(Renderer::transformedSunDirection3, 8, 3);

		UI::drawFPS(FPS);
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
					objects.push_back(newBullet);
					bullets.push_back(newBullet);
					SoundEngine->play2D("audio/shoot.wav", false);
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
		switch (action)
		{
		case PlayerAction::toggleInfo:
			showInfo = !showInfo;
			break;
		case PlayerAction::interact:
			// todo
			break;
		case PlayerAction::toggleFlashlight:
			players[0]->toggleFlashlight();
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


		if (gameState == GameState::GAME_MENU)
		{
			if (action == PlayerAction::moveForward)
			{
				for (int i = 0; i < UI::getMenuItemList().size(); i++)
				{
					if (UI::getMenuItemList()[i]->selected)
					{
						UI::getMenuItemList()[i]->selected = false;
						UI::getMenuItemList()[(i - 1) % UI::getMenuItemList().size()]->selected = true;
						break;
					}
				}
				SoundEngine->play2D("audio/select.wav", false);
			}

			if (action == PlayerAction::moveBackward)
			{
				for (int i = 0; i < UI::getMenuItemList().size(); i++)
				{
					if (UI::getMenuItemList()[i]->selected)
					{
						UI::getMenuItemList()[i]->selected = false;
						UI::getMenuItemList()[(i + 1) % UI::getMenuItemList().size()]->selected = true;
						break;
					}
				}
				SoundEngine->play2D("audio/select.wav", false);
			}

			if (action == PlayerAction::enter || action == PlayerAction::jump)
			{
				MenuItem* selectedMenuItem = UI::getSelectedMenuItem();
				if (selectedMenuItem->type == MenuItemType::resume)
				{
					toggleMenu();
				}
				if (selectedMenuItem->type == MenuItemType::exit)
				{
					SDL_DestroyWindow(window);
					exit(0);
				}
				if (selectedMenuItem->type == MenuItemType::restart)
				{
					Map::restart(&objects, &characters, &players, &npcs);
					toggleMenu();
					gameState = GameState::GAME_ACTIVE;
				}
				SoundEngine->play2D("audio/select.wav", false);
			}
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
	std::string enteredText="";

	std::cin >> enteredText;

	if (enteredText[0] == 'x')
	{
		players[0]->setPosition(glm::vec3(std::stof(enteredText.substr(1)), players[0]->getPosition().y, players[0]->getPosition().z) );
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