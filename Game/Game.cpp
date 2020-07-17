#include "Game.h"

//#include <chrono>
//#include <thread>



#define STB_IMAGE_IMPLEMENTATION
#include "libs/stb_image.h"
#undef STB_IMAGE_IMPLEMENTATION



SDL_Window* Game::window;

std::vector<Object*> Game::objects;
std::vector<Character*> Game::characters;
std::vector<Player*> Game::players;
std::vector<NPC*> Game::npcs;
std::vector<Bullet*> Game::bullets;


bool Game::pressedKeys[20];

float Game::time = 0.0f;
uint32  Game::FPS = 0;
bool  Game::close = false;

uint64 Game::perfCounterFrequency;
uint64 Game::lastCounter;
float32 Game::delta;

void Game::startGame()
{
	perfCounterFrequency = SDL_GetPerformanceFrequency();
	lastCounter = SDL_GetPerformanceCounter();
	delta = 0.0f;


	SDL_SetRelativeMouseMode(SDL_TRUE);

	gameLoop();
}


void Game::init()
{
	Renderer::initOpenGL(&window);
	Renderer::initShader();

	UI::init(window);

	Renderer::showLoadingScreen();



	std::string levelname = ConfigManager::readConfig("level");
	ResourceManager::loadMap("levels/" + levelname, &objects, &characters, &players, &npcs);

	Renderer::init(players[0]);
}

void Game::gameLoop()
{
	while (!close)
	{
		time += delta;


		for (Object* object : objects)
		{
			object->calculationBeforeFrame();
		}

		processInput();

		for (NPC* npc : npcs)
		{
			//npc->followCharacter(delta / 1000, objects, players[0]);
			//npc->followNavPoints(delta / 1000, objects);
			npc->doCurrentTask(delta / 1000, objects, characters);
		}

		for (Bullet* bullet : bullets)
		{
			bullet->move(delta / 1000);
			bullet->fall(delta / 1000);
			bullet->checkHit(objects);
		}

		//Move every Object
		for (Object* object : objects)
		{
			if (object->getType() == ObjectType::Object_Environment) continue; //Environment doesnt move
			if (object->getType() == ObjectType::Object_Bullet) continue; //Bullets have their own move function

			object->move(delta / 1000, objects[1]);
			object->fall(delta / 1000);
		}

		//Check every Object for collision
		for (Object* object : objects)
		{
			CollisionResult collisionResult = object->checkCollision(objects);
			if (!collisionResult.collided) continue;

			if (object->getType() & ObjectType::Object_Character && collisionResult.onTop == true)
			{
				Character* character = static_cast<Character*>(object);
				character->activateJumping();
			}

			if ((object->getType() & ObjectType::Object_NPC) && (collisionResult.collidedWith->getType() & ObjectType::Object_Entity | ObjectType::Object_Character))
			{
				Logger::log("test");
				NPC* npc = static_cast<NPC*>(object);
				npc->evade(delta / 1000, objects);
			}

		}

		for (Player* player : players)
		{
			player->updateCameraPosition();
			//player->update();
		}

		for (Character* character : characters)
		{
			character->resetVerticalMovement();
		}

		deleteObjects();

		render();

		for (Object* object : objects)
		{
			object->calculationAfterFrame();
		}

		//std::chrono::duration<int, std::milli> timespan(100);
		//std::this_thread::sleep_for(timespan);

		uint64 endCounter = SDL_GetPerformanceCounter();
		uint64 counterElasped = endCounter - lastCounter;
		delta = ((float32)counterElasped) / (float32)perfCounterFrequency;
		FPS = (uint32)((float32)perfCounterFrequency / (float32)counterElasped);
		lastCounter = endCounter;

	}
}

void Game::render()
{
	GLCALL(glClearColor(0.0f, 0.0f, 0.8f, 1.0f));
	GLCALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

	Renderer::calcLight(players[0]);

	Renderer::renderSkybox(players[0]);

	Renderer::renderObjects(players[0], objects);

	UI::drawFPS(FPS);
	UI::drawPos(players[0]);
	UI::drawRot(players[0]);
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
			}
		}
		else if (event.type == SDL_MOUSEBUTTONDOWN) {
			if (event.button.button == SDL_BUTTON_LEFT) {
				if (SDL_GetRelativeMouseMode()) {
					Bullet* newBullet = players[0]->shoot();
					objects.push_back(newBullet);
					bullets.push_back(newBullet);
				}

				SDL_SetRelativeMouseMode(SDL_TRUE);
			}
		}
	}

	if (pressedKeys[PlayerAction::moveForward])
	{
		players[0]->moveForward();
	}
	if (pressedKeys[PlayerAction::moveBackward])
	{
		players[0]->moveBackward();
	}
	if (pressedKeys[PlayerAction::moveLeft])
	{
		players[0]->moveLeft();
	}
	if (pressedKeys[PlayerAction::moveRight])
	{
		players[0]->moveRight();
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
	if (pressedKeys[PlayerAction::toggleFlashlight])
	{
		players[0]->activateFlashlight(true);
	}
	if (!pressedKeys[PlayerAction::toggleFlashlight])
	{
		players[0]->activateFlashlight(false);
	}
	if (pressedKeys[PlayerAction::toggleInfo])
	{

	}
	if (pressedKeys[PlayerAction::toggleWireframe])
	{
		GLCALL(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE));
	}
	if (!pressedKeys[PlayerAction::toggleWireframe])
	{
		GLCALL(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));
	}
	if (pressedKeys[PlayerAction::pause])
	{

	}
	if (pressedKeys[PlayerAction::menu])
	{
		SDL_SetRelativeMouseMode(SDL_FALSE);
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
			objects.erase(objects.begin() + i);
			//i++;
		}
	}
}
