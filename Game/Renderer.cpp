#include "Renderer.h"

#include "Game.h"
#include "ResourceManager.h"
#include "UI.h"
#include "Player.h"

#ifdef _DEBUG
void openGLDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
	std::string messageS = message;
	Logger::log("[OpenGL Callback] " + messageS);
}

void _GLGetError(const char* file, int line, const char* call) {
	while (GLenum error = glGetError())
	{
		Logger::log("[OpenGL Error] " + std::string((char*)glewGetErrorString(error)) + " in " + std::string(file) + ":" + std::to_string(line) + " Call: " + std::string(call));
	}
}
#define GLCALL(call) call; _GLGetError(__FILE__, __LINE__, #call)
#else
#define GLCALL(call) call
#endif

float screenVertices[] = {
	// positions          
	-1.0f,	-1.0f,	0.0f,	0.0f, 0.0f,
	-1.0f,	+1.0f,	0.0f,	0.0f, 1.0f,
	1.0f,	-1.0f,	0.0f,	1.0f, 0.0f,

	1.0f,	-1.0f,	0.0f,	1.0f, 0.0f,
	-1.0f,	+1.0f,	0.0f,	0.0f, 1.0f,
	1.0f,   1.0f,	0.0f,	1.0f, 1.0f,
};

float skyboxVertices[] = {
	// positions          
	-1.0f,  1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	-1.0f,  1.0f, -1.0f,
	 1.0f,  1.0f, -1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f,  1.0f
};

float axisVertices[] = {
	// positions										//colors
	0.0f * 9 / 16,		0.0f,		0.0f * 9 / 16,		0.2f,	0.0f,	0.0f,	1.0f,
	1.0f * 9 / 16,		0.0f,		0.0f * 9 / 16,		1.0f,	0.0f,	0.0f,	1.0f,

	0.0f * 9 / 16,		0.0f,		0.0f * 9 / 16,		0.0f,	0.2f,	0.0f,	1.0f,
	0.0f * 9 / 16,		1.0f,		0.0f * 9 / 16,		0.0f,	1.0f,	0.0f,	1.0f,

	0.0f * 9 / 16,		0.0f,		0.0f * 9 / 16,		0.0f,	0.0f,	0.2f,	1.0f,
	0.0f * 9 / 16,		0.0f,		1.0f * 9 / 16,		0.0f,	0.0f,	1.0f,	1.0f,
};


Shader* Renderer::shaderSkybox = nullptr;
Shader* Renderer::shaderBasic = nullptr;
Shader* Renderer::shaderImage = nullptr;
Shader* Renderer::shaderGeometry = nullptr;
Shader* Renderer::shaderPostProcessing = nullptr;
Shader* Renderer::shaderDepthMap = nullptr;

unsigned int Renderer::loadingScreenTexture;
UI_Element_ProgressBar* Renderer::loadingProgressBar;

unsigned int Renderer::skyboxTexture;
VertexBuffer* Renderer::skyboxVertexBuffer;
VertexBuffer* Renderer::axisVertexBuffer;
VertexBuffer* Renderer::screenVertexBuffer;

std::vector<Model*> Renderer::models;

int Renderer::skyboxViewUniformIndex;
int Renderer::skyboxProjUniformIndex;
int Renderer::modelUniformIndex;
int Renderer::viewUniformIndex;
int Renderer::projUniformIndex;
int Renderer::lightdirectionUniformIndex;
int Renderer::lightpositionUniformIndex;
int Renderer::envmapUniformIndex;
int Renderer::lightspacematrixUniformIndex;
int Renderer::shadowmapUniformIndex;

glm::vec3 Renderer::sunDirection;
glm::vec3 Renderer::spotLightPosition;
glm::vec4 Renderer::pointLightPosition;

glm::mat4 Renderer::modelViewProj;

bool Renderer::wireframeMode = false;
bool Renderer::showNormalMode = false;

glm::vec3 Renderer::transformedSunDirection3;

FrameBuffer Renderer::frameBuffer;
FrameBuffer Renderer::depthMapBuffer;

int Renderer::shadowMapResolution;

std::vector<float32> Renderer::postprocessingEffectDuration = { 0,0,0 };


void Renderer::initOpenGL()
{
	SDL_Init(SDL_INIT_EVERYTHING);

	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	/*SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 8);*/

	//SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);


#ifdef _DEBUG
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
#endif // DEBUG

	uint32 flags = SDL_WINDOW_OPENGL;

	int resolution_width = 0, resolution_height = 0;

	int fullscreen = std::stoi(ConfigManager::readConfig("fullscreen"));
	if (fullscreen == 1)
	{
		flags += SDL_WINDOW_FULLSCREEN_DESKTOP;
		resolution_width = std::stoi(ConfigManager::readConfig("fullscreen_resolution_width"));
		resolution_height = std::stoi(ConfigManager::readConfig("fullscreen_resolution_height"));
	}
	else
	{
		resolution_width = std::stoi(ConfigManager::readConfig("windowed_resolution_width"));
		resolution_height = std::stoi(ConfigManager::readConfig("windowed_resolution_height"));
	}



	Game::window = SDL_CreateWindow("OpenGL-Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, resolution_width, resolution_height, flags);
	SDL_GLContext glContext = SDL_GL_CreateContext(Game::window);

	int vsync = std::stoi(ConfigManager::readConfig("v_sync"));
	SDL_GL_SetSwapInterval(vsync);

	GLenum err = glewInit();
	if (err != GLEW_OK)
	{
		Logger::log("Error: " + std::string((char*)glewGetErrorString(err)));
		std::cin.get();
		exit;
	}
	Logger::log("OpenGL version: " + std::string((char*)glGetString(GL_VERSION)));

#ifdef _DEBUG
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(openGLDebugCallback, 0);
#endif // DEBUG

	GLCALL(glEnable(GL_CULL_FACE));
	GLCALL(glEnable(GL_DEPTH_TEST));
	GLCALL(glFrontFace(GL_CCW));

	GLCALL(glEnable(GL_BLEND));
	GLCALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

	GLCALL(glClearColor(0, 0, 0, 0));

	initLoadingScreen();
}

void Renderer::loadShader()
{
	shaderSkybox = ResourceManager::loadShader("shaders/skybox.vert", "shaders/skybox.frag");
	shaderBasic = ResourceManager::loadShader("shaders/basic.vert", "shaders/basic.frag");
	shaderImage = ResourceManager::loadShader("shaders/image.vert", "shaders/image.frag");
	shaderGeometry = ResourceManager::loadShader("shaders/geometry.vert", "shaders/geometry.frag");
	shaderPostProcessing = ResourceManager::loadShader("shaders/postprocessing.vert", "shaders/postprocessing.frag");
	shaderDepthMap = ResourceManager::loadShader("shaders/depthmap.vert", "shaders/depthmap.frag");
}

void Renderer::init()
{
	skyboxVertexBuffer = new VertexBuffer(skyboxVertices, 36, VertexType::_VertexPos);
	axisVertexBuffer = new VertexBuffer(axisVertices, 6, VertexType::_VertexPosCol);

	shadowMapResolution = std::stoi(ConfigManager::readConfig("shadow_map_resolution"));

	initFrameBuffer();

	std::vector<std::string> faces
	{
			"textures/skybox/skybox1_right.jpg",
			"textures/skybox/skybox1_left.jpg",
			"textures/skybox/skybox1_top.jpg",
			"textures/skybox/skybox1_bottom.jpg",
			"textures/skybox/skybox1_front.jpg",
			"textures/skybox/skybox1_back.jpg"
	};
	skyboxTexture = ResourceManager::loadCubemap(faces);

	//get Uniform location indices for passing data to the GPU
	skyboxViewUniformIndex = GLCALL(glGetUniformLocation(shaderSkybox->getShaderId(), "u_view"));
	skyboxProjUniformIndex = GLCALL(glGetUniformLocation(shaderSkybox->getShaderId(), "u_proj"));

	modelUniformIndex = GLCALL(glGetUniformLocation(shaderBasic->getShaderId(), "u_model"));
	viewUniformIndex = GLCALL(glGetUniformLocation(shaderBasic->getShaderId(), "u_view"));
	projUniformIndex = GLCALL(glGetUniformLocation(shaderBasic->getShaderId(), "u_proj"));

	envmapUniformIndex = GLCALL(glGetUniformLocation(shaderBasic->getShaderId(), "u_env_map"));
	lightspacematrixUniformIndex = GLCALL(glGetUniformLocation(shaderBasic->getShaderId(), "u_light_space_matrix"));
	shadowmapUniformIndex = GLCALL(glGetUniformLocation(shaderBasic->getShaderId(), "u_shadow_map"));

	initLight();
}

void Renderer::initFrameBuffer()
{
	frameBuffer.create(Game::getWindowWidth(), Game::getWindowHeight(), FrameBufferTextureType::colorMap | FrameBufferTextureType::stencilMap);
	depthMapBuffer.create(shadowMapResolution, shadowMapResolution, FrameBufferTextureType::depthMap);
}

void Renderer::initLight()
{
	shaderBasic->bind();


	lightdirectionUniformIndex = GLCALL(glGetUniformLocation(shaderBasic->getShaderId(), "u_directional_light.direction"));
	//glm::vec3 sunColor = glm::vec3(0.98f, 0.83f, 0.30f);
	glm::vec3 sunColor = glm::vec3(0.8);
	sunDirection = glm::vec3(-0.8, +0.4, +0.4);
	GLCALL(glUniform3fv(glGetUniformLocation(shaderBasic->getShaderId(), "u_directional_light.diffuse"), 1, (float*)&sunColor));
	GLCALL(glUniform3fv(glGetUniformLocation(shaderBasic->getShaderId(), "u_directional_light.specular"), 1, (float*)&sunColor));
	sunColor = glm::vec3(0.3);
	GLCALL(glUniform3fv(glGetUniformLocation(shaderBasic->getShaderId(), "u_directional_light.ambient"), 1, (float*)&sunColor));


	glm::vec3 pointLightColor = glm::vec3(0, 0, 0);
	GLCALL(glUniform3fv(glGetUniformLocation(shaderBasic->getShaderId(), "u_point_light.diffuse"), 1, (float*)&pointLightColor));
	GLCALL(glUniform3fv(glGetUniformLocation(shaderBasic->getShaderId(), "u_point_light.specular"), 1, (float*)&pointLightColor));
	pointLightColor *= 0.2f;
	GLCALL(glUniform3fv(glGetUniformLocation(shaderBasic->getShaderId(), "u_point_light.ambient"), 1, (float*)&pointLightColor));
	GLCALL(glUniform1f(glGetUniformLocation(shaderBasic->getShaderId(), "u_point_light.linear"), 0.007f));
	GLCALL(glUniform1f(glGetUniformLocation(shaderBasic->getShaderId(), "u_point_light.quadratic"), 0.0002));
	pointLightPosition = glm::vec4(80, 2.5, 10, 1.0f);
	lightpositionUniformIndex = GLCALL(glGetUniformLocation(shaderBasic->getShaderId(), "u_point_light.position"));


	glm::vec3 spotLightColor = glm::vec3(0);
	GLCALL(glUniform3fv(glGetUniformLocation(shaderBasic->getShaderId(), "u_spot_light.diffuse"), 1, (float*)&spotLightColor));
	GLCALL(glUniform3fv(glGetUniformLocation(shaderBasic->getShaderId(), "u_spot_light.specular"), 1, (float*)&spotLightColor));
	spotLightColor *= 0.2f;
	GLCALL(glUniform3fv(glGetUniformLocation(shaderBasic->getShaderId(), "u_spot_light.ambient"), 1, (float*)&spotLightColor));
	spotLightPosition = glm::vec3(0.0f);
	GLCALL(glUniform3fv(glGetUniformLocation(shaderBasic->getShaderId(), "u_spot_light.position"), 1, (float*)&spotLightPosition));
	spotLightPosition.z = 1.0f;
	GLCALL(glUniform3fv(glGetUniformLocation(shaderBasic->getShaderId(), "u_spot_light.direction"), 1, (float*)&spotLightPosition));
	GLCALL(glUniform1f(glGetUniformLocation(shaderBasic->getShaderId(), "u_spot_light.innerCone"), 0.95f));
	GLCALL(glUniform1f(glGetUniformLocation(shaderBasic->getShaderId(), "u_spot_light.outerCone"), 0.80f));

	GLCALL(glUniform1i(glGetUniformLocation(shaderBasic->getShaderId(), "u_showNormalMode"), 0));
}

void Renderer::initLoadingScreen()
{
	screenVertexBuffer = new VertexBuffer(screenVertices, 6, VertexType::_VertexPosTex);

	loadingProgressBar = new UI_Element_ProgressBar(Game::getWindowWidth() / 2 - 200, Game::getWindowHeight() / 2 - 10, 400, 20, 0, 0, false);
	UI::addElement(loadingProgressBar);
	loadingScreenTexture = ResourceManager::loadImage("images/loading_screen.png");
}

void Renderer::drawLoadingScreen() {

	GLCALL(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
	GLCALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

	renderImage(screenVertexBuffer, loadingScreenTexture);

	UI::drawUI();

	SDL_GL_SwapWindow(Game::window);
}


void Renderer::calcLight()
{

	//Directionallight
	//glm::vec4 transformedSunDirection = glm::vec4(sunDirection, 1.0f);
	glm::vec4 transformedSunDirection = -glm::transpose(glm::inverse(Game::players[0]->getView())) * glm::vec4(sunDirection, 1.0f);
	transformedSunDirection3 = glm::vec3(transformedSunDirection.x, transformedSunDirection.y, transformedSunDirection.z);

	//Logger::log("Sun Direction: x:"+std::to_string(transformedSunDirection.x)+" y:" + std::to_string(transformedSunDirection.y) + " z:" + std::to_string(transformedSunDirection.y));

	shaderBasic->bind();
	glUniform3fv(lightdirectionUniformIndex, 1, (float*)&transformedSunDirection);

	//Pointlight
	glm::mat4 pointLightMatrix = glm::mat4(1.0f);
	pointLightPosition = pointLightMatrix * pointLightPosition;
	glm::vec3 transformedPointLightPosition = (glm::vec3) (Game::players[0]->getView() * pointLightPosition);
	glUniform3fv(lightpositionUniformIndex, 1, (float*)&transformedPointLightPosition);

	shaderBasic->unbind();
}

void Renderer::calcShadows()
{
	depthMapBuffer.bind();

	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	glViewport(0, 0, shadowMapResolution, shadowMapResolution);
	glClear(GL_DEPTH_BUFFER_BIT);
	glCullFace(GL_BACK);

	renderShadowsMap();

	depthMapBuffer.unbind();
	// 2. then render scene as normal with shadow mapping (using depth map)
	glViewport(0, 0, Game::getWindowWidth(), Game::getWindowHeight()); //actual resolution
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	GLCALL(glActiveTexture(GL_TEXTURE3));
	GLCALL(glBindTexture(GL_TEXTURE_2D, depthMapBuffer.getTextureId()[2]));


	shaderBasic->bind();

	float near_plane = -150, far_plane = 250;
	glm::mat4 depthProjectionMatrix = glm::ortho(-150.0f, 150.0f, -150.0f, 150.0f, near_plane, far_plane);
	glm::mat4 depthViewMatrix = glm::lookAt(sunDirection, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	glm::mat4 lightSpaceMatrix = depthProjectionMatrix * depthViewMatrix;

	GLCALL(glUniformMatrix4fv(lightspacematrixUniformIndex, 1, GL_FALSE, &lightSpaceMatrix[0][0]));

	GLCALL(glUniform1i(shadowmapUniformIndex, 3));

}

void Renderer::renderShadowsMap()
{
	glCullFace(GL_FRONT);

	float near_plane = -150, far_plane = 250;
	glm::mat4 depthProjectionMatrix = glm::ortho(-150.0f, 150.0f, -150.0f, 150.0f, near_plane, far_plane);
	glm::mat4 depthViewMatrix = glm::lookAt(sunDirection, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

	glm::mat4 lightSpaceMatrix = depthProjectionMatrix * depthViewMatrix;


	shaderDepthMap->bind();
	int lightSpaceMatrixLocation = GLCALL(glGetUniformLocation(shaderDepthMap->getShaderId(), "u_light_space_matrix"));
	GLCALL(glUniformMatrix4fv(lightSpaceMatrixLocation, 1, GL_FALSE, &lightSpaceMatrix[0][0]));

	shaderBasic->bind();
	int lightSpaceMatrixLocation1 = GLCALL(glGetUniformLocation(shaderBasic->getShaderId(), "u_light_space_matrix"));
	GLCALL(glUniformMatrix4fv(lightSpaceMatrixLocation1, 1, GL_FALSE, &lightSpaceMatrix[0][0]));

	shaderDepthMap->bind();

	for (std::shared_ptr<Object> object : Game::objects)
	{
		//if (object->getType() & ObjectType::Object_Player) continue;
		//if (object->getType() & ObjectType::Object_Environment) continue;

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
		int modelUniformLocation = GLCALL(glGetUniformLocation(shaderDepthMap->getShaderId(), "u_model"));
		GLCALL(glUniformMatrix4fv(modelUniformLocation, 1, GL_FALSE, &model[0][0]));


		object->renderShadowMap();

	}

	shaderDepthMap->unbind();
}

void Renderer::showShadowMap()
{
	shaderImage->bind();
	GLCALL(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
	GLCALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

	//loadingScreenTexture = ResourceManager::loadImage("images/loading_screen.png");

	renderImage(screenVertexBuffer, depthMapBuffer.getTextureId()[2]);

	//SDL_GL_SwapWindow(*window);
}

void Renderer::renderSkybox()
{
	shaderSkybox->bind();
	skyboxVertexBuffer->bind();

	glCullFace(GL_BACK);
	glDepthFunc(GL_LEQUAL);

	glm::mat4 view = glm::mat4(glm::mat3(Game::players[0]->getView()));
	glm::mat4 proj = Game::players[0]->getProj();
	glUniformMatrix4fv(skyboxViewUniformIndex, 1, GL_FALSE, &view[0][0]);
	glUniformMatrix4fv(skyboxProjUniformIndex, 1, GL_FALSE, &proj[0][0]);

	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	glDepthFunc(GL_LESS);

	skyboxVertexBuffer->unbind();
	shaderSkybox->unbind();
}

void Renderer::renderImage(VertexBuffer* imageVertexBuffer, int imageIndex)
{
	shaderImage->bind();
	imageVertexBuffer->bind();


	GLCALL(glDepthMask(GL_FALSE));
	GLCALL(glDisable(GL_CULL_FACE));
	GLCALL(glDisable(GL_DEPTH_TEST));

	GLCALL(glActiveTexture(GL_TEXTURE0));
	GLCALL(glBindTexture(GL_TEXTURE_2D, imageIndex));
	GLCALL(glDrawArrays(GL_TRIANGLES, 0, 6));

	GLCALL(glDepthMask(GL_TRUE));
	GLCALL(glEnable(GL_CULL_FACE));
	GLCALL(glEnable(GL_DEPTH_TEST));


	imageVertexBuffer->unbind();
	shaderImage->unbind();
}

void Renderer::renderObjects()
{
	GLCALL(glCullFace(GL_BACK));

	shaderBasic->bind();
	int cameraposUniformIndex = GLCALL(glGetUniformLocation(shaderBasic->getShaderId(), "u_camerapos"));
	GLCALL(glUniform3fv(cameraposUniformIndex, 1, (float*) &Game::players[0]->getCameraPosition()));


	for (int i = 0; i <= 1; i++)
	{
		for (std::shared_ptr<Object> object : Game::objects)
		{
			if (i == 0 && object->getModel()->getHasTransparentTexture()) continue;
			if (i == 1 && !object->getModel()->getHasTransparentTexture()) continue;


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
			modelViewProj = Game::players[0]->getViewProj() * model;
			glm::mat4 modelView = Game::players[0]->getView() * model;
			glm::mat4 invModelView = glm::transpose(glm::inverse(modelView));


			glm::mat4 view = Game::players[0]->getView();
			glm::mat4 proj = Game::players[0]->getProj();

			object->bindShader();

			int isGettingDamagedUniformLocation = GLCALL(glGetUniformLocation(shaderBasic->getShaderId(), "u_isgettingdamaged"));
			if (object->isGettingDamaged()) {
				
				GLCALL(glUniform1i(isGettingDamagedUniformLocation, 1));
			}
			else {
				GLCALL(glUniform1i(isGettingDamagedUniformLocation, 0));
			}

			int modelUniformLocation = GLCALL(glGetUniformLocation(shaderBasic->getShaderId(), "u_model"));
			GLCALL(glUniformMatrix4fv(modelUniformLocation, 1, GL_FALSE, &model[0][0]));

			//per object a different envmap can be used
			GLCALL(glActiveTexture(GL_TEXTURE2));
			GLCALL(glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture));
			GLCALL(glUniform1i(envmapUniformIndex, 2));

			GLCALL(glUniformMatrix4fv(modelUniformIndex, 1, GL_FALSE, &model[0][0]));
			GLCALL(glUniformMatrix4fv(viewUniformIndex, 1, GL_FALSE, &view[0][0]));
			GLCALL(glUniformMatrix4fv(projUniformIndex, 1, GL_FALSE, &proj[0][0]));

			object->render();

			object->unbindShader();
		}
	}
}

void Renderer::renderTransparentObjects()
{
	GLCALL(glCullFace(GL_BACK));

	shaderBasic->bind();
	int cameraposUniformIndex = GLCALL(glGetUniformLocation(shaderBasic->getShaderId(), "u_camerapos"));
	GLCALL(glUniform3fv(cameraposUniformIndex, 1, (float*)&Game::players[0]->getCameraPosition()));

	//Logger::log("Camera position: " + std::to_string(Game::players[0]->getCameraPosition().x) + " " + std::to_string(Game::players[0]->getCameraPosition().y) + " " + std::to_string(Game::players[0]->getCameraPosition().z));

		for (std::shared_ptr<Object> object : Game::objects)
		{
			if (!object->getModel()->getHasTransparentTexture()) continue;


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
			modelViewProj = Game::players[0]->getViewProj() * model;
			glm::mat4 modelView = Game::players[0]->getView() * model;
			glm::mat4 invModelView = glm::transpose(glm::inverse(modelView));


			glm::mat4 view = Game::players[0]->getView();
			glm::mat4 proj = Game::players[0]->getProj();

			object->bindShader();

			int isGettingDamagedUniformLocation = GLCALL(glGetUniformLocation(shaderBasic->getShaderId(), "u_isgettingdamaged"));
			if (object->isGettingDamaged()) {

				GLCALL(glUniform1i(isGettingDamagedUniformLocation, 1));
			}
			else {
				GLCALL(glUniform1i(isGettingDamagedUniformLocation, 0));
			}

			int modelUniformLocation = GLCALL(glGetUniformLocation(shaderBasic->getShaderId(), "u_model"));
			GLCALL(glUniformMatrix4fv(modelUniformLocation, 1, GL_FALSE, &model[0][0]));

			//per object a different envmap can be used
			GLCALL(glActiveTexture(GL_TEXTURE2));
			GLCALL(glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture));
			GLCALL(glUniform1i(envmapUniformIndex, 2));

			GLCALL(glUniformMatrix4fv(modelUniformIndex, 1, GL_FALSE, &model[0][0]));
			GLCALL(glUniformMatrix4fv(viewUniformIndex, 1, GL_FALSE, &view[0][0]));
			GLCALL(glUniformMatrix4fv(projUniformIndex, 1, GL_FALSE, &proj[0][0]));

			object->render();

			object->unbindShader();
		}
}

void Renderer::renderOpaqueObjects()
{
	GLCALL(glCullFace(GL_BACK));

	shaderBasic->bind();
	int cameraposUniformIndex = GLCALL(glGetUniformLocation(shaderBasic->getShaderId(), "u_camerapos"));
	GLCALL(glUniform3fv(cameraposUniformIndex, 1, (float*)&Game::players[0]->getCameraPosition()));

	for (std::shared_ptr<Object> object : Game::objects)
	{
		if (object->getModel()->getHasTransparentTexture()) continue;


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
		modelViewProj = Game::players[0]->getViewProj() * model;
		glm::mat4 modelView = Game::players[0]->getView() * model;
		glm::mat4 invModelView = glm::transpose(glm::inverse(modelView));


		glm::mat4 view = Game::players[0]->getView();
		glm::mat4 proj = Game::players[0]->getProj();

		object->bindShader();

		int isGettingDamagedUniformLocation = GLCALL(glGetUniformLocation(shaderBasic->getShaderId(), "u_isgettingdamaged"));
		if (object->isGettingDamaged()) {

			GLCALL(glUniform1i(isGettingDamagedUniformLocation, 1));
		}
		else {
			GLCALL(glUniform1i(isGettingDamagedUniformLocation, 0));
		}

		int modelUniformLocation = GLCALL(glGetUniformLocation(shaderBasic->getShaderId(), "u_model"));
		GLCALL(glUniformMatrix4fv(modelUniformLocation, 1, GL_FALSE, &model[0][0]));

		//per object a different envmap can be used
		GLCALL(glActiveTexture(GL_TEXTURE2));
		GLCALL(glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture));
		GLCALL(glUniform1i(envmapUniformIndex, 2));

		GLCALL(glUniformMatrix4fv(modelUniformIndex, 1, GL_FALSE, &model[0][0]));
		GLCALL(glUniformMatrix4fv(viewUniformIndex, 1, GL_FALSE, &view[0][0]));
		GLCALL(glUniformMatrix4fv(projUniformIndex, 1, GL_FALSE, &proj[0][0]));

		object->render();

		object->unbindShader();
	}
}

void Renderer::renderAxis(glm::vec3 vector, int x, int y)
{
	shaderGeometry->bind();
	axisVertexBuffer->bind();

	GLCALL(glDepthMask(GL_FALSE));
	GLCALL(glDisable(GL_CULL_FACE));
	GLCALL(glDisable(GL_DEPTH_TEST));


	glm::mat4 model = glm::mat4(1.0f);

	//scale the model
	model = glm::scale(model, glm::vec3(0.1f));

	//move to position of model
	model = glm::translate(model, glm::vec3(x, y, 0));



	//rotate model around X
	float gegk = vector.y;
	float ank = sqrt(pow(vector.x, 2) + pow(vector.z, 2));
	float angle = atan2(gegk, ank);
	model = glm::rotate(model, -angle, glm::vec3(1, 0, 0));

	//rotate model around Y
	//model = glm::rotate(model, (float)M_PI, glm::vec3(0, 1, 0));
	angle = atan2(vector.x, vector.z);
	model = glm::rotate(model, (float)(-angle + 1 * M_PI), glm::vec3(0, 1, 0));


	int modelUniformIndex = GLCALL(glGetUniformLocation(shaderGeometry->getShaderId(), "u_model"));
	GLCALL(glUniformMatrix4fv(modelUniformIndex, 1, GL_FALSE, &model[0][0]));

	GLCALL(glDrawArrays(GL_LINES, 0, 6));

	GLCALL(glDepthMask(GL_TRUE));
	GLCALL(glEnable(GL_CULL_FACE));
	GLCALL(glEnable(GL_DEPTH_TEST));


	axisVertexBuffer->unbind();
	shaderGeometry->unbind();
}

void Renderer::setModels(std::vector<Model*> newModels)
{
	models = newModels;
}

std::vector<Model*> Renderer::getModels()
{
	return models;
}

Shader* Renderer::getShader(ShaderType shadertype)
{
	switch (shadertype)
	{
	case ShaderType::basic:
		return shaderBasic;
	case ShaderType::skybox:
		return shaderSkybox;
	}
}

void Renderer::toggleWireframe()
{
	wireframeMode = !wireframeMode;
	if (wireframeMode)
	{
		GLCALL(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE));
	}
	else
	{
		GLCALL(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));

	}
}

void Renderer::toggleShowNormals()
{
	shaderBasic->bind();

	showNormalMode = !showNormalMode;
	if (showNormalMode)
	{
		GLCALL(glUniform1i(glGetUniformLocation(shaderBasic->getShaderId(), "u_showNormalMode"), 1));
	}
	else
	{
		GLCALL(glUniform1i(glGetUniformLocation(shaderBasic->getShaderId(), "u_showNormalMode"), 0));
	}

}

void Renderer::postProcessing()
{
	shaderPostProcessing->bind();

	for (int i = 0; i < postprocessingEffectDuration.size(); i++)
	{
		if (postprocessingEffectDuration[i] > 0)
			postprocessingEffectDuration[i] -= Game::getDelta();
	}

	if (postprocessingEffectDuration[PostProcessingEffect::blood] > 0)
	{
		GLCALL(glUniform1i(glGetUniformLocation(shaderPostProcessing->getShaderId(), "u_postprocessingeffect.blood"), 1));
	}
	else
	{
		GLCALL(glUniform1i(glGetUniformLocation(shaderPostProcessing->getShaderId(), "u_postprocessingeffect.blood"), 0));
	}

	if (postprocessingEffectDuration[PostProcessingEffect::negative] > 0)
	{
		GLCALL(glUniform1i(glGetUniformLocation(shaderPostProcessing->getShaderId(), "u_postprocessingeffect.negative"), 1));
	}
	else
	{
		GLCALL(glUniform1i(glGetUniformLocation(shaderPostProcessing->getShaderId(), "u_postprocessingeffect.negative"), 0));
	}

	if (postprocessingEffectDuration[PostProcessingEffect::uncolored] > 0)
	{
		GLCALL(glUniform1i(glGetUniformLocation(shaderPostProcessing->getShaderId(), "u_postprocessingeffect.uncolored"), 1));
	}
	else
	{
		GLCALL(glUniform1i(glGetUniformLocation(shaderPostProcessing->getShaderId(), "u_postprocessingeffect.uncolored"), 0));
	}




	GLCALL(glActiveTexture(GL_TEXTURE0));
	GLCALL(glBindTexture(GL_TEXTURE_2D, frameBuffer.getTextureId()[0]));
	GLCALL(glUniform1i(glGetUniformLocation(shaderPostProcessing->getShaderId(), "u_texture"), 0));
	glDrawArrays(GL_TRIANGLES, 0, 3);
	shaderPostProcessing->unbind();
}

void Renderer::clearBuffer()
{
	shaderBasic->bind();
	GLCALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
}

void Renderer::applyPostprocessingEffect(PostProcessingEffect postprocessingeffect, float32 duration)
{
	postprocessingEffectDuration[postprocessingeffect] = duration;
}

