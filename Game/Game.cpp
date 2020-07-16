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

bool  Game::spaceReleased = true;

float Game::time = 0.0f;
uint32  Game::FPS = 0;
bool  Game::close = false;

uint64 Game::perfCounterFrequency;
uint64 Game::lastCounter;
float32 Game::delta;

int Game::modelViewProjMatrixUniformIndex;
int Game::modelViewUniformIndex;
int Game::invmodelViewUniformIndex;

int Game::directionLocation;

glm::vec3 Game::sunDirection;
glm::vec3 Game::spotLightPosition;
glm::vec4 Game::pointLightPosition;
int Game::positionLocation;

glm::mat4 Game::modelViewProj;



void Game::startGame()
{
	perfCounterFrequency = SDL_GetPerformanceFrequency();
	lastCounter = SDL_GetPerformanceCounter();
	delta = 0.0f;

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::scale(model, glm::vec3(1.0f));

	modelViewProj = players[0]->getViewProj() * model;


	modelViewProjMatrixUniformIndex = GLCALL(glGetUniformLocation(ResourceManager::getObjectShader()->getShaderId(), "u_modelViewProj"));
	modelViewUniformIndex = GLCALL(glGetUniformLocation(ResourceManager::getObjectShader()->getShaderId(), "u_modelView"));
	invmodelViewUniformIndex = GLCALL(glGetUniformLocation(ResourceManager::getObjectShader()->getShaderId(), "u_invModelView"));

	SDL_SetRelativeMouseMode(SDL_TRUE);

	gameLoop();
}

void Game::initOpenGL()
{
	SDL_Init(SDL_INIT_EVERYTHING);

	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetSwapInterval(1);

	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 8);

	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);


#ifdef _DEBUG
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
#endif // DEBUG

	uint32 flags = SDL_WINDOW_OPENGL;
	int fullscreen = std::stoi(ConfigManager::readConfig("fullscreen"));
	if (fullscreen == 1)
	{
		flags += SDL_WINDOW_FULLSCREEN_DESKTOP;
	}

	int resolution_width = std::stoi(ConfigManager::readConfig("resolution_width"));
	int resolution_height = std::stoi(ConfigManager::readConfig("resolution_height"));

	window = SDL_CreateWindow("OpenGL-Test", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, resolution_width, resolution_height, flags);
	SDL_GLContext glContext = SDL_GL_CreateContext(window);

	GLenum err = glewInit();
	if (err != GLEW_OK)
	{
		Logger::log("Error: " + std::string((char*)glewGetErrorString(err)));
		std::cin.get();
		exit;
	}
	Logger::log("OpenGL version: " + std::string((char*)glGetString(GL_VERSION)));

#ifdef _DEBUG123
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(openGLDebugCallback, 0);
#endif // DEBUG

	GLCALL(glEnable(GL_CULL_FACE));
	GLCALL(glEnable(GL_DEPTH_TEST));
	GLCALL(glFrontFace(GL_CCW));

	GLCALL(glEnable(GL_BLEND));
	GLCALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

}

void Game::initEverythingElse()
{


	UI::init(window);

	ResourceManager::init();
	ResourceManager::bindShader();

	directionLocation = GLCALL(glGetUniformLocation(ResourceManager::getObjectShader()->getShaderId(), "u_directional_light.direction"));
	glm::vec3 sunColor = glm::vec3(0.98f, 0.83f, 0.30f);
	//sunColor *= 0.4f;
	sunDirection = glm::vec3(-1.0f);
	GLCALL(glUniform3fv(glGetUniformLocation(ResourceManager::getObjectShader()->getShaderId(), "u_directional_light.diffuse"), 1, (float*)&sunColor));
	GLCALL(glUniform3fv(glGetUniformLocation(ResourceManager::getObjectShader()->getShaderId(), "u_directional_light.specular"), 1, (float*)&sunColor));
	sunColor *= 0.2f;
	GLCALL(glUniform3fv(glGetUniformLocation(ResourceManager::getObjectShader()->getShaderId(), "u_directional_light.ambient"), 1, (float*)&sunColor));


	glm::vec3 pointLightColor = glm::vec3(0, 0, 1);
	GLCALL(glUniform3fv(glGetUniformLocation(ResourceManager::getObjectShader()->getShaderId(), "u_point_light.diffuse"), 1, (float*)&pointLightColor));
	GLCALL(glUniform3fv(glGetUniformLocation(ResourceManager::getObjectShader()->getShaderId(), "u_point_light.specular"), 1, (float*)&pointLightColor));
	pointLightColor *= 0.2f;
	GLCALL(glUniform3fv(glGetUniformLocation(ResourceManager::getObjectShader()->getShaderId(), "u_point_light.ambient"), 1, (float*)&pointLightColor));
	GLCALL(glUniform1f(glGetUniformLocation(ResourceManager::getObjectShader()->getShaderId(), "u_point_light.linear"), 0.007f));
	GLCALL(glUniform1f(glGetUniformLocation(ResourceManager::getObjectShader()->getShaderId(), "u_point_light.quadratic"), 0.0002));
	pointLightPosition = glm::vec4(80, 2.5, 10, 1.0f);
	positionLocation = GLCALL(glGetUniformLocation(ResourceManager::getObjectShader()->getShaderId(), "u_point_light.position"));


	glm::vec3 spotLightColor = glm::vec3(0);
	GLCALL(glUniform3fv(glGetUniformLocation(ResourceManager::getObjectShader()->getShaderId(), "u_spot_light.diffuse"), 1, (float*)&spotLightColor));
	GLCALL(glUniform3fv(glGetUniformLocation(ResourceManager::getObjectShader()->getShaderId(), "u_spot_light.specular"), 1, (float*)&spotLightColor));
	spotLightColor *= 0.2f;
	GLCALL(glUniform3fv(glGetUniformLocation(ResourceManager::getObjectShader()->getShaderId(), "u_spot_light.ambient"), 1, (float*)&spotLightColor));
	spotLightPosition = glm::vec3(0.0f);
	GLCALL(glUniform3fv(glGetUniformLocation(ResourceManager::getObjectShader()->getShaderId(), "u_spot_light.position"), 1, (float*)&spotLightPosition));
	spotLightPosition.z = 1.0f;
	GLCALL(glUniform3fv(glGetUniformLocation(ResourceManager::getObjectShader()->getShaderId(), "u_spot_light.direction"), 1, (float*)&spotLightPosition));
	GLCALL(glUniform1f(glGetUniformLocation(ResourceManager::getObjectShader()->getShaderId(), "u_spot_light.innerCone"), 0.95f));
	GLCALL(glUniform1f(glGetUniformLocation(ResourceManager::getObjectShader()->getShaderId(), "u_spot_light.outerCone"), 0.80f));



	std::string levelname = ConfigManager::readConfig("level");
	ResourceManager::loadMap("levels/" + levelname, &objects, &characters, &players, &npcs);

	Renderer::init();
}

void Game::gameLoop()
{
	while (!close)
	{
		time += delta;
		GLCALL(glClearColor(0.0f, 0.0f, 0.8f, 1.0f));
		GLCALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

		for (Object* object : objects)
		{
			object->calculationBeforeFrame();
		}


		ResourceManager::bindShader();
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
			if (object->getType() == ObjectType::Object_Environment) continue;
			if (object->getType() == ObjectType::Object_Bullet) continue;

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
			player->update();
		}

		for (Character* character : characters)
		{
			character->resetVerticalMovement();
		}

		deleteObjects();


		glm::vec4 transformedSunDirection = glm::transpose(glm::inverse(players[0]->getView())) * glm::vec4(sunDirection, 1.0f);
		glUniform3fv(directionLocation, 1, (float*)&transformedSunDirection);

		glm::mat4 pointLightMatrix = glm::mat4(1.0f);
		pointLightPosition = pointLightMatrix * pointLightPosition;
		glm::vec3 transformedPointLightPosition = (glm::vec3) (players[0]->getView() * pointLightPosition);
		glUniform3fv(positionLocation, 1, (float*)&transformedPointLightPosition);


		render();


		UI::drawFPS(FPS);
		UI::drawPos(players[0]);
		UI::drawUI();


		for (Object* object : objects)
		{
			object->calculationAfterFrame();
		}




		SDL_GL_SwapWindow(window);

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
	Renderer::renderSkybox(players[0]);

	for (Object* object: objects)
	{
		glm::mat4 model = glm::mat4(1.0f);
		//model = glm::scale(model, glm::vec3(1.0f));

		//move to position of model
		model = glm::translate(model, object->getPosition());

		//rotate model around X
		float angle = object->getRotation().x;
		model = glm::rotate(model, glm::radians(angle), glm::vec3(1, 0, 0));

		//rotate model around Y
		angle = object->getRotation().y;
		model = glm::rotate(model, glm::radians(angle), glm::vec3(0, 1, 0));

		//rotate model around z
		angle = object->getRotation().z;
		model = glm::rotate(model, glm::radians(angle), glm::vec3(0, 0, 1));

		//view and projection
		modelViewProj = players[0]->getViewProj() * model;
		glm::mat4 modelView = players[0]->getView() * model;
		glm::mat4 invModelView = glm::transpose(glm::inverse(modelView));

		object->bindShader();

		GLCALL(glUniformMatrix4fv(modelViewProjMatrixUniformIndex, 1, GL_FALSE, &modelViewProj[0][0]));
		GLCALL(glUniformMatrix4fv(modelViewUniformIndex, 1, GL_FALSE, &modelView[0][0]));
		GLCALL(glUniformMatrix4fv(invmodelViewUniformIndex, 1, GL_FALSE, &invModelView[0][0]));

		object->render();
	}
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
				glm::vec3 spotLightColor = glm::vec3(1.0f);
				GLCALL(glUniform3fv(glGetUniformLocation(ResourceManager::getObjectShader()->getShaderId(), "u_spot_light.diffuse"), 1, (float*)&spotLightColor));
				GLCALL(glUniform3fv(glGetUniformLocation(ResourceManager::getObjectShader()->getShaderId(), "u_spot_light.specular"), 1, (float*)&spotLightColor));
				spotLightColor *= 0.2f;
				GLCALL(glUniform3fv(glGetUniformLocation(ResourceManager::getObjectShader()->getShaderId(), "u_spot_light.ambient"), 1, (float*)&spotLightColor));
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
				spaceReleased = true;
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
				glm::vec3 spotLightColor = glm::vec3(0);
				GLCALL(glUniform3fv(glGetUniformLocation(ResourceManager::getObjectShader()->getShaderId(), "u_spot_light.diffuse"), 1, (float*)&spotLightColor));
				GLCALL(glUniform3fv(glGetUniformLocation(ResourceManager::getObjectShader()->getShaderId(), "u_spot_light.specular"), 1, (float*)&spotLightColor));
				GLCALL(glUniform3fv(glGetUniformLocation(ResourceManager::getObjectShader()->getShaderId(), "u_spot_light.ambient"), 1, (float*)&spotLightColor));
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
	if (buttonSpace && spaceReleased) {
		players[0]->jump();
		spaceReleased = false;
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

