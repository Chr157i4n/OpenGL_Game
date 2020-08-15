#include "Game.h"

std::vector<std::shared_ptr<Object>> Game::map;
std::vector<std::shared_ptr<Object>> Game::objects;
std::vector< std::shared_ptr<Character> > Game::characters;
std::vector< std::shared_ptr<NPC> > Game::npcs;
std::vector< std::shared_ptr<Bullet> > Game::bullets;

std::vector<std::shared_ptr<Client>>	Game::clients;

bool  Game::close = false;

float32 Game::delta = 0;

GameState Game::gameState = GameState::GAME_MENU;


StopWatch Game::stopwatch1;
StopWatch Game::gameStopWatch;
StopWatch Game::frametimeStopWatch;


/// <summary>
/// this methods starts the game
/// </summary>
void Game::startGame()
{
	delta = 0.0f;
	gameState = GameState::GAME_ACTIVE;
	gameStopWatch.start();
}


/// <summary>
/// this function inits everything before the game can be started
/// </summary>
void Game::init()
{
	ConfigManager::init("config.ini");
	Map::load(ConfigManager::level);
	NetworkManager::init();
	gameLoop();
}


void Game::initMultiplayer()
{
	NetworkManager::init();
	//NetworkManager::connect();
}

/// <summary>
/// main game loop
/// </summary>
void Game::gameLoop()
{
	while (!close)
	{
		frametimeStopWatch.start();

		#pragma region gameloop

		NetworkManager::waitForEvent();

		if (gameState == GameState::GAME_ACTIVE || gameState == GameState::GAME_GAME_OVER)
		{

			


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

			//Move every Object
			for (std::shared_ptr<Object> object : objects)
			{
				if (!object->getEnabled()) continue;
				if (object->getType() & ObjectType::Object_Environment) continue; //Environment doesnt move


				
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

			for (std::shared_ptr<Character> character : characters)
			{
				if (!character->getEnabled()) continue;
				character->resetVerticalMovement();
			}

			deleteObjects();


			for (std::shared_ptr<Object> object : objects) 
			{
				if (!object->getEnabled()) continue;
				object->calculationAfterFrame();
			}

		}

		#pragma endregion

		float64 frametime = frametimeStopWatch.stop();

		if (frametime < 1000 / ConfigManager::fps_limit_server && ConfigManager::fps_limit_server != 0)
		{
			sleep(1000 / ConfigManager::fps_limit_server - frametime);
		}

		delta = frametimeStopWatch.stop();

		//Logger::log("delta: " + std::to_string(delta));
		//Logger::log("fps: " + std::to_string(1000/delta));

	}
	NetworkManager::deinit();
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
/// deletes or disable object, that are destroyed
/// </summary>
void Game::deleteObjects()
{
	for (int i = objects.size() - 1; i >= 0; i--)
	{
		if (objects[i]->getHealth() <= 0 && objects[i]->getEnabled())
		{
			objects[i]->disable();
			NetworkManager::broadcastData(NetworkCommand::disable_object, NetworkManager::networkIDtoString(objects[i]) + "|0");
			
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

void Game::sleep(int ms)
{
	std::chrono::duration<double, std::milli> delta_ms(ms);
	auto delta_ms_duration = std::chrono::duration_cast<std::chrono::milliseconds>(delta_ms);
	std::this_thread::sleep_for(std::chrono::milliseconds(delta_ms_duration));
}
