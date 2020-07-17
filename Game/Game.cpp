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


bool  Game::buttonW = false;
bool  Game::buttonS = false;
bool  Game::buttonA = false;
bool  Game::buttonD = false;
bool  Game::buttonSpace = false;
bool  Game::buttonShift = false;
bool  Game::buttonCtrl = false;

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


		Renderer::getShader(ShaderType::basic)->bind();

		input();

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
				Character* character= static_cast<Character*>(object);
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
	Renderer::calcLight(players[0]);

	Renderer::renderSkybox(players[0]);

	Renderer::renderObjects(players[0], objects);

	UI::drawFPS(FPS);
	UI::drawPos(players[0]);
	UI::drawRot(players[0]);
	UI::drawUI();

	SDL_GL_SwapWindow(window);
}

void Game::input()
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
			switch (event.key.keysym.sym)
			{
			case SDLK_w:
				buttonW = true;
				break;
			case SDLK_s:
				buttonS = true;
				break;
			case SDLK_a:
				buttonA = true;
				break;
			case SDLK_d:
				buttonD = true;
				break;
			case SDLK_SPACE:
				buttonSpace = true;
				break;
			case SDLK_LCTRL:
				buttonCtrl = true;
				break;
			case SDLK_LSHIFT:
				buttonShift = true;
				break;
			case SDLK_p:
				GLCALL(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE));
				break;
			case SDLK_f:
				players[0]->activateFlashlight(true);
				break;
			case SDLK_ESCAPE:
				SDL_SetRelativeMouseMode(SDL_FALSE);
				break;


			}
		}
		else if (event.type == SDL_KEYUP)
		{
			switch (event.key.keysym.sym)
			{
			case SDLK_w:
				buttonW = false;
				break;
			case SDLK_s:
				buttonS = false;
				break;
			case SDLK_a:
				buttonA = false;
				break;
			case SDLK_d:
				buttonD = false;
				break;
			case SDLK_SPACE:
				buttonSpace = false;
				break;
			case SDLK_LSHIFT:
				buttonShift = false;
				break;
			case SDLK_LCTRL:
				buttonCtrl = false;
				break;
			case SDLK_p:
				GLCALL(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));
				break;
			case SDLK_f:
				players[0]->activateFlashlight(false);
				break;
			}
		}
		else if (event.type == SDL_MOUSEMOTION) {
			if (SDL_GetRelativeMouseMode()) {
				players[0]->onMouseMove(event.motion.xrel, event.motion.yrel);
			}
		}
		else if (event.type == SDL_MOUSEBUTTONDOWN) {
			if (event.button.button == SDL_BUTTON_LEFT) {
				SDL_SetRelativeMouseMode(SDL_TRUE);
				Bullet* newBullet = players[0]->shoot();
				objects.push_back(newBullet);
				bullets.push_back(newBullet);
			}
		}

	}

	if (buttonW) {
		players[0]->moveForward(objects);
	}
	if (buttonS) {
		players[0]->moveBackward(objects);
	}
	if (buttonA) {
		players[0]->moveLeft(objects);
	}
	if (buttonD) {
		players[0]->moveRight(objects);
	}
	if (buttonSpace) {
		players[0]->jump();
	}
	if (buttonCtrl) {
		players[0]->crouch(true);
	}
	if (!buttonCtrl) {
		players[0]->crouch(false);
	}
	if (buttonShift) {
		players[0]->run(true);
	}
	if (!buttonShift) {
		players[0]->run(false);
	}
}

void Game::deleteObjects()
{
	for (int i = objects.size()-1; i >= 0; i--)
	{
		if (objects[i]->getHealth() <= 0)
		{
			objects.erase(objects.begin() + i);
			//i++;
		}
	}
}

