#include "Game.h"

//#include <chrono>
//#include <thread>



#define STB_IMAGE_IMPLEMENTATION
#include "libs/stb_image.h"
#undef STB_IMAGE_IMPLEMENTATION




//#define DEBUG_COLLISION
//#define DEBUG_OUTOFBOUNDS
//#define DEBUG_GRAVITY

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

bool Game::showInfo=false;
GameState Game::gameState;
int Game::maxBulletCount = 20;

irrklang::ISoundEngine* Game::SoundEngine = irrklang::createIrrKlangDevice();

void Game::startGame()
{
	perfCounterFrequency = SDL_GetPerformanceFrequency();
	lastCounter = SDL_GetPerformanceCounter();
	delta = 0.0f;
	gameState = GameState::GAME_ACTIVE;

	SDL_SetRelativeMouseMode(SDL_TRUE);

	float musicvolume = std::stof(ConfigManager::readConfig("musicvolume"));
	SoundEngine->setSoundVolume(musicvolume/100);

	irrklang::ISound* music = SoundEngine->play2D("audio/breakout.mp3", true);



	gameLoop();
}


void Game::init()
{
	Renderer::initOpenGL(&window);
	Renderer::initShader();

	UI::init(window);

	Renderer::showLoadingScreen();


	maxBulletCount = stoi(ConfigManager::readConfig("max_bullets"));
	std::string levelname = ConfigManager::readConfig("level");
	ResourceManager::loadMap("levels/" + levelname, &objects, &characters, &players, &npcs);

	Renderer::init(players[0]);
}

void Game::gameLoop()
{
	while (!close)
	{
		time += delta;

		processInput();

		if (gameState == GameState::GAME_ACTIVE)
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

			for (std::shared_ptr<Bullet> bullet : bullets)
			{
				bullet->fall(delta / 1000);
				bullet->move(delta / 1000);
				bullet->checkHit(objects);
			}

			//Move every Object
			for (std::shared_ptr<Object> object : objects)
			{
				if (object->getType() == ObjectType::Object_Environment) continue; //Environment doesnt move
				if (object->getType() == ObjectType::Object_Bullet) continue; //Bullets have their own move function
				
				object->fall(delta / 1000);
				object->move(delta / 1000, objects[1]);
			}

			//Check every Object for collision
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

			}

			for (std::shared_ptr<Player> player : players)
			{
				player->updateCameraPosition();
				//player->update();
				if (player->getPosition().x > 95 && player->getPosition().z > 95)
				{
					UI_Element* eastereggLabel = new UI_Element{ UI::getWidth()/2-200 , UI::getHeight()/2-200,"Nice, du hast das Easter-Egg gefunden",glm::vec4(1,0,0,1),false };
					UI::addElement(eastereggLabel);
				}
			}

			for (std::shared_ptr<Character> character : characters)
			{
				character->resetVerticalMovement();
			}

			deleteObjects();

			if (npcs.size() <= 0)
			{
				UI_Element* victoryLabel = new UI_Element{ UI::getWidth() / 2 - 100 , UI::getHeight() / 2,"Du hast alle Bots besigt",glm::vec4(0,0,1,1),false };
				UI::addElement(victoryLabel);
			}

			render();

			for (std::shared_ptr<Object> object : objects)
			{
				object->calculationAfterFrame();
			}

			//std::chrono::duration<int, std::milli> timespan(100);
			//std::this_thread::sleep_for(timespan);

			SDL_GL_SwapWindow(window);
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
		FPS = (uint32)((float32)perfCounterFrequency / (float32)counterElasped);

		//Logger::log("FPS: " + std::to_string(FPS));
		lastCounter = endCounter;

	}
}

void Game::render()
{

	Renderer::calcLight(players[0]);

	Renderer::frameBuffer.bind();
	GLCALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
	Renderer::renderSkybox(players[0]);
	Renderer::renderObjects(players[0], objects);
	Renderer::frameBuffer.unbind();

	//Postprocessing
	Renderer::postProcessing();


	if (showInfo)
	{
		Renderer::renderAxis(players[0]->getLookDirection(), 8, 5);
		Renderer::renderAxis(Renderer::transformedSunDirection3, 8, 3);

		UI::drawFPS(FPS);
		UI::drawPos(players[0]);
		UI::drawRot(players[0]);
		
	}

	UI::drawUI();
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
		if (action == PlayerAction::toggleInfo)
		{
			showInfo = !showInfo;
		}
		if (action == PlayerAction::toggleFlashlight)
		{
			players[0]->toggleFlashlight();
		}
		if (action == PlayerAction::toggleWireframe)
		{
			Renderer::toggleWireframe();
		}
		if (action == PlayerAction::toggleShowNormals)
		{
			Renderer::toggleShowNormals();
		}
		if (action == PlayerAction::pause)
		{
			togglePause();
		}
		if (action == PlayerAction::menu)
		{
			toggleMenu();
		}
		if (action == PlayerAction::toggleFullscreen)
		{
			toggleFullscreen();
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

			if (action == PlayerAction::enter)
			{
				MenuItem* selectedMenuItem = UI::getSelectedMenuItem();
				if (selectedMenuItem->type==MenuItemType::resume)
				{
					toggleMenu();
				}
				if (selectedMenuItem->type == MenuItemType::exit)
				{
					SDL_DestroyWindow(window);
					exit(0);
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
	if (gameState == GameState::GAME_PAUSED || gameState == GameState::GAME_ACTIVE)
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
}

void Game::updateAudioListener()
{
	irrklang::vec3df Lposition = irrklang::vec3df(players[0]->getPosition().x, players[0]->getPosition().y, players[0]->getPosition().z);
	irrklang::vec3df LlookAt = irrklang::vec3df(-players[0]->getLookDirection().x, players[0]->getLookDirection().y, -players[0]->getLookDirection().z);
	Game::SoundEngine->setListenerPosition(Lposition, LlookAt);
}