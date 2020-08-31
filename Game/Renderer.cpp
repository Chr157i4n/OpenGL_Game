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

GLenum  cube_map_axis1[6] = {
	GL_TEXTURE_CUBE_MAP_POSITIVE_X,
	GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
	GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
	GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
	GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
	GL_TEXTURE_CUBE_MAP_NEGATIVE_Z };

GLenum fboAttachment1[6] = {
	GL_COLOR_ATTACHMENT0,
	GL_COLOR_ATTACHMENT0,
	GL_COLOR_ATTACHMENT0,
	GL_COLOR_ATTACHMENT0,
	GL_COLOR_ATTACHMENT0,
	GL_COLOR_ATTACHMENT0 };



Shader* Renderer::shaderSkybox = nullptr;
Shader* Renderer::shaderBasic = nullptr;
Shader* Renderer::shaderImage = nullptr;
Shader* Renderer::shaderGeometry = nullptr;
Shader* Renderer::shaderPostProcessing = nullptr;
Shader* Renderer::shaderShadowMap = nullptr;
Shader* Renderer::shaderEnvMap = nullptr;

UI_Element_ProgressBar* Renderer::loadingProgressBar;

unsigned int Renderer::loadingScreenTexture;
unsigned int Renderer::main_menu_backgroundTexture;

VertexBuffer* Renderer::skyboxVertexBuffer;
VertexBuffer* Renderer::axisVertexBuffer;
VertexBuffer* Renderer::screenVertexBuffer;

std::vector<Model*> Renderer::models;

int Renderer::skyboxViewUniformIndex;
int Renderer::skyboxProjUniformIndex;
int Renderer::modelUniformIndex;
int Renderer::viewUniformIndex;
int Renderer::projUniformIndex;
int Renderer::lightdirectionBasicUniformIndex;
int Renderer::lightdirectionEnvUniformIndex;
int Renderer::envmapBasicUniformIndex;
int Renderer::envmapEnvUniformIndex;
int Renderer::lightspacematrixUniformIndex;
int Renderer::shadowmapUniformIndex;
int Renderer::shadowmodeUniformIndex;

glm::vec3 Renderer::spotLightPosition;

glm::mat4 Renderer::modelViewProj;

bool Renderer::wireframeMode = false;
bool Renderer::showNormalMode = false;

glm::vec3 Renderer::transformedSunDirection3;

FrameBuffer Renderer::frameBuffer;
FrameBuffer Renderer::shadowMapBuffer;
FrameBuffer Renderer::envMapFacesBuffer[6];
FrameBuffer Renderer::envMapBuffer;

std::vector<float32> Renderer::postprocessingEffectDuration = { 0,0,0 };

int Renderer::frameCount = 0;


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

	if (ConfigManager::fullscreen_option == FullscreenOption::fullscreen)
	{
		flags += SDL_WINDOW_FULLSCREEN_DESKTOP;
	}


	Game::window = SDL_CreateWindow("OpenGL-Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, getResolutionX(), getResolutionY(), flags);
	SDL_GLContext glContext = SDL_GL_CreateContext(Game::window);

	SDL_GL_SetSwapInterval(ConfigManager::v_sync);

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
	shaderShadowMap = ResourceManager::loadShader("shaders/depthmap.vert", "shaders/depthmap.frag");
	shaderEnvMap = ResourceManager::loadShader("shaders/envmap.vert", "shaders/envmap.frag");
}

void Renderer::init()
{
	skyboxVertexBuffer = new VertexBuffer(skyboxVertices, 36, VertexType::_VertexPos);
	axisVertexBuffer = new VertexBuffer(axisVertices, 6, VertexType::_VertexPosCol);


	Renderer::changeResolution(Game::getWindowWidth(), Game::getWindowHeight());
	//initFrameBuffer();

	main_menu_backgroundTexture = ResourceManager::loadImage("images/main_menu_background.png");

	//get Uniform location indices for passing data to the GPU
	shaderBasic->bind();
	skyboxViewUniformIndex = GLCALL(glGetUniformLocation(shaderSkybox->getShaderId(), "u_view"));
	skyboxProjUniformIndex = GLCALL(glGetUniformLocation(shaderSkybox->getShaderId(), "u_proj"));

	modelUniformIndex = GLCALL(glGetUniformLocation(shaderBasic->getShaderId(), "u_model"));
	viewUniformIndex = GLCALL(glGetUniformLocation(shaderBasic->getShaderId(), "u_view"));
	projUniformIndex = GLCALL(glGetUniformLocation(shaderBasic->getShaderId(), "u_proj"));

	envmapBasicUniformIndex = GLCALL(glGetUniformLocation(shaderBasic->getShaderId(), "u_env_map"));
	lightspacematrixUniformIndex = GLCALL(glGetUniformLocation(shaderBasic->getShaderId(), "u_light_space_matrix"));
	shadowmapUniformIndex = GLCALL(glGetUniformLocation(shaderBasic->getShaderId(), "u_shadow_map"));

	shadowmodeUniformIndex = GLCALL(glGetUniformLocation(shaderBasic->getShaderId(), "u_shadow_mode"));
	Renderer::toggleShadows(ConfigManager::shadow_option);

	shaderEnvMap->bind();
	envmapEnvUniformIndex = GLCALL(glGetUniformLocation(shaderEnvMap->getShaderId(), "u_env_map"));


	//Renderer::initLight();

	Renderer::initFrameBuffer();

}

void Renderer::initFrameBuffer()
{
	//delete old framebuffer
	frameBuffer.destroy();
	shadowMapBuffer.destroy();
	for (std::shared_ptr<Object> object : Game::objects)
	{
		if (object->getType() & ObjectType::Object_Character) continue;

		unsigned int cubemap = object->getEnvCubeMap();
		glDeleteTextures(1, &cubemap);
		object->setEnvCubeMap(-1);

		unsigned int framebufferid = object->getEnvCubeMapFrameBuffer();
		glDeleteFramebuffers(1, &framebufferid);
		object->setEnvCubeMapFrameBuffer(-1);

		unsigned int depthbufferid = object->getEnvCubeMapDepthBuffer();
		glDeleteRenderbuffers(1, &depthbufferid);
		object->setEnvCubeMapDepthBuffer(-1);
	}

	//create them new
	for (std::shared_ptr<Object> object : Game::objects)
	{
		if (object->getType() & ObjectType::Object_Character) continue;

		unsigned int  framebuffer, depthbuffer;

		// create the cubemap
		unsigned int cubemap;
		glGenTextures(1, &cubemap);
		object->setEnvCubeMap(cubemap);

		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		// set textures
		for (int i = 0; i < 6; ++i)
			glTexImage2D(cube_map_axis1[i], 0, GL_RGB, ConfigManager::env_map_resolution, ConfigManager::env_map_resolution, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

		// create the fbo
		glGenFramebuffers(1, &framebuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

		object->setEnvCubeMapFrameBuffer(framebuffer);

		// create the uniform depth buffer
		glGenRenderbuffers(1, &depthbuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, depthbuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, ConfigManager::env_map_resolution, ConfigManager::env_map_resolution);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		// attach it
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthbuffer);

		object->setEnvCubeMapDepthBuffer(depthbuffer);
	}

	frameBuffer.create(Renderer::getResolutionX(), Renderer::getResolutionY(), FrameBufferTextureType::colorMap | FrameBufferTextureType::stencilMap);
	shadowMapBuffer.create(ConfigManager::shadow_map_resolution, ConfigManager::shadow_map_resolution, FrameBufferTextureType::depthMap);

}

void Renderer::initLight()
{
#pragma region shaderBasic

	shaderBasic->bind();

	//Directional Light
	lightdirectionBasicUniformIndex = GLCALL(glGetUniformLocation(shaderBasic->getShaderId(), "u_directional_light.direction"));
	GLCALL(glUniform3fv(glGetUniformLocation(shaderBasic->getShaderId(), "u_directional_light.color"), 1, (float*)&Map::sun_color));


	for (int i = 0; i < 10; i++)
	{
		glm::vec3 zero = glm::vec3(0, 0, 0);
		GLCALL(glUniform3fv(glGetUniformLocation(shaderBasic->getShaderId(), ("u_point_lights[" + std::to_string(i) + "].position").c_str()), 1, (float*) &zero));
		GLCALL(glUniform3fv(glGetUniformLocation(shaderBasic->getShaderId(), ("u_point_lights[" + std::to_string(i) + "].color").c_str()), 1, (float*)&zero));
		GLCALL(glUniform1f(glGetUniformLocation(shaderBasic->getShaderId(), ("u_point_lights[" + std::to_string(i) + "].linear").c_str()), 0));
		GLCALL(glUniform1f(glGetUniformLocation(shaderBasic->getShaderId(), ("u_point_lights[" + std::to_string(i) + "].quadratic").c_str()), 0));
	}

	//Point Light
	for (int i = 0; i < Map::pointLights.size(); i++)
	{
		Map::pointLights[i].positionUniformIndex = GLCALL(glGetUniformLocation(shaderBasic->getShaderId(), ("u_point_lights[" + std::to_string(i) + "].position").c_str()));

		Map::pointLights[i].colorUniformIndex = GLCALL(glGetUniformLocation(shaderBasic->getShaderId(), ("u_point_lights[" + std::to_string(i) + "].color").c_str()));

		GLCALL(glUniform3fv(Map::pointLights[i].colorUniformIndex, 1, (float*)&Map::pointLights[i].color));
		GLCALL(glUniform1f(glGetUniformLocation(shaderBasic->getShaderId(), ("u_point_lights[" + std::to_string(i) + "].linear").c_str()), Map::pointLights[i].linear));
		GLCALL(glUniform1f(glGetUniformLocation(shaderBasic->getShaderId(), ("u_point_lights[" + std::to_string(i) + "].quadratic").c_str()), Map::pointLights[i].quadratic));
	}


	glm::vec3 spotLightColor = glm::vec3(0);
	GLCALL(glUniform3fv(glGetUniformLocation(shaderBasic->getShaderId(), "u_spot_light.color"), 1, (float*)&spotLightColor));
	spotLightPosition = glm::vec3(0.0f);
	GLCALL(glUniform3fv(glGetUniformLocation(shaderBasic->getShaderId(), "u_spot_light.position"), 1, (float*)&spotLightPosition));
	spotLightPosition.z = 1.0f;
	GLCALL(glUniform3fv(glGetUniformLocation(shaderBasic->getShaderId(), "u_spot_light.direction"), 1, (float*)&spotLightPosition));
	GLCALL(glUniform1f(glGetUniformLocation(shaderBasic->getShaderId(), "u_spot_light.innerCone"), 0.95f));
	GLCALL(glUniform1f(glGetUniformLocation(shaderBasic->getShaderId(), "u_spot_light.outerCone"), 0.80f));

	GLCALL(glUniform1i(glGetUniformLocation(shaderBasic->getShaderId(), "u_showNormalMode"), 0));

#pragma endregion

#pragma region shaderEnvMap

	shaderEnvMap->bind();

	lightdirectionEnvUniformIndex = GLCALL(glGetUniformLocation(shaderEnvMap->getShaderId(), "u_directional_light.direction"));
	GLCALL(glUniform3fv(glGetUniformLocation(shaderEnvMap->getShaderId(), "u_directional_light.color"), 1, (float*)&Map::sun_color));
	GLCALL(glUniform3fv(lightdirectionEnvUniformIndex, 1, (float*)&Map::sun_direction));
#pragma endregion
}

void Renderer::initLoadingScreen()
{
	screenVertexBuffer = new VertexBuffer(screenVertices, 6, VertexType::_VertexPosTex);

	loadingProgressBar = new UI_Element_ProgressBar(Game::getWindowWidth() / 2 - 200, Game::getWindowHeight() / 2 - 10, 400, 20, 0, 0, glm::vec4(0, 0, 1, 1));
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

void Renderer::drawMainMenuBackground()
{
	renderImage(screenVertexBuffer, main_menu_backgroundTexture);
}


void Renderer::calcLight()
{

	//Directionallight
	//glm::vec4 transformedSunDirection = glm::vec4(sunDirection, 1.0f);
	glm::vec4 transformedSunDirection = -glm::transpose(glm::inverse(Game::players[0]->getView())) * glm::vec4(Map::sun_direction, 1.0f);
	transformedSunDirection3 = glm::vec3(transformedSunDirection.x, transformedSunDirection.y, transformedSunDirection.z);


	shaderBasic->bind();
	glUniform3fv(lightdirectionBasicUniformIndex, 1, (float*)&transformedSunDirection);

	//Pointlight
	for (int i = 0; i < Map::pointLights.size(); i++)
	{
		glm::vec3 transformedPointLightPosition = (glm::vec3) (Game::players[0]->getView() * glm::vec4(Map::pointLights[i].position,1));
		glUniform3fv(Map::pointLights[i].positionUniformIndex, 1, (float*)&transformedPointLightPosition);
	}

	shaderBasic->unbind();
}

void Renderer::calcShadows()
{
	shadowMapBuffer.bind();

	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	glViewport(0, 0, ConfigManager::shadow_map_resolution, ConfigManager::shadow_map_resolution);
	glClear(GL_DEPTH_BUFFER_BIT);
	//glCullFace(GL_BACK);

	renderShadowsMap();

	shadowMapBuffer.unbind();
	// 2. then render scene as normal with shadow mapping (using depth map)
	glViewport(0, 0, Game::getWindowWidth(), Game::getWindowHeight()); //actual resolution
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	shaderBasic->bind();

	GLCALL(glActiveTexture(GL_TEXTURE2));
	GLCALL(glBindTexture(GL_TEXTURE_2D, shadowMapBuffer.getTextureId()[2]));
	GLCALL(glUniform1i(shadowmapUniformIndex, 2));

}

void Renderer::renderShadowsMap()
{
	glCullFace(GL_FRONT);

	float near_plane = -150, far_plane = 250;
	glm::mat4 depthProjectionMatrix = glm::ortho(-150.0f, 150.0f, -150.0f, 150.0f, near_plane, far_plane);
	glm::mat4 depthViewMatrix = glm::lookAt(Map::sun_direction, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

	glm::mat4 lightSpaceMatrix = depthProjectionMatrix * depthViewMatrix;

	shaderBasic->bind();
	int lightSpaceMatrixLocation1 = GLCALL(glGetUniformLocation(shaderBasic->getShaderId(), "u_light_space_matrix"));
	GLCALL(glUniformMatrix4fv(lightSpaceMatrixLocation1, 1, GL_FALSE, &lightSpaceMatrix[0][0]));

	shaderShadowMap->bind();
	int lightSpaceMatrixLocation = GLCALL(glGetUniformLocation(shaderShadowMap->getShaderId(), "u_light_space_matrix"));
	GLCALL(glUniformMatrix4fv(lightSpaceMatrixLocation, 1, GL_FALSE, &lightSpaceMatrix[0][0]));



	for (std::shared_ptr<Object> object : Game::objects)
	{
		if (!object->getEnabled()) continue;
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
		int modelUniformLocation = GLCALL(glGetUniformLocation(shaderShadowMap->getShaderId(), "u_model"));
		GLCALL(glUniformMatrix4fv(modelUniformLocation, 1, GL_FALSE, &model[0][0]));


		object->renderShadowMap();

	}

	shaderShadowMap->unbind();
}

void Renderer::showShadowMap()
{
	shaderImage->bind();
	GLCALL(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
	GLCALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

	//loadingScreenTexture = ResourceManager::loadImage("images/loading_screen.png");

	renderImage(screenVertexBuffer, shadowMapBuffer.getTextureId()[2]);
	//renderImage(screenVertexBuffer, shadowMapBuffer.getTextureId()[3]);

	//SDL_GL_SwapWindow(*window);
}

void Renderer::DrawCircle(float cx, float cy, float r, int num_segments)
{
	glBegin(GL_LINE_LOOP);
	for (int ii = 0; ii < num_segments; ii++)
	{
		float theta = (2.0f * 3.1415926f * float(ii) / float(num_segments));//get the current angle

		float x = r * cosf(theta);//calculate the x component
		float y = r * sinf(theta);//calculate the y component

		glVertex2f(x + cx, y + cy);//output vertex

	}
	glEnd();
}

void Renderer::DrawFilledCircle(float cx, float cy, float r, int num_segments)
{
	glBegin(GL_TRIANGLE_FAN);
	for (int ii = 0; ii < num_segments; ii++)
	{
		float theta = (2.0f * 3.1415926f * float(ii) / float(num_segments));//get the current angle

		float x = r * cosf(theta);//calculate the x component
		float y = r * sinf(theta);//calculate the y component

		glVertex2f(x + cx, y + cy);//output vertex

	}
	glEnd();
}

void Renderer::renderMap()
{
	glm::mat4 view, proj;
	float angle = 90.0f;

	glViewport((Renderer::getResolutionX()-Renderer::getResolutionY())/2,0,Renderer::getResolutionY(), Renderer::getResolutionY());
	
	float near_plane = -100, far_plane = 10;
	float viewsize=0.55f;
	if (Map::getMapSize().x > Map::getMapSize().y)
	{
		viewsize *= Map::getMapSize().x;
	}
	else {
		viewsize *= Map::getMapSize().y;
	}
	proj = glm::ortho(-viewsize, viewsize, -viewsize, viewsize, near_plane, far_plane);
	//proj = glm::perspective(glm::radians(angle), 1.0f, -40.0f, 40.0f);
	view = glm::lookAt(Game::objects[1]->getCenter(), Game::objects[1]->getCenter() + glm::vec3(0, -1, 0), glm::vec3(0, 0, -1));

	shaderEnvMap->bind();



	GLCALL(glCullFace(GL_BACK));
	GLCALL(glClearColor(0, 0, 0, 0));
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Directionallight
	//glm::vec4 transformedSunDirection = glm::vec4(sunDirection, 1.0f);
	glm::vec4 transformedSunDirection = -glm::transpose(glm::inverse(view)) * glm::vec4(Map::sun_direction, 1.0f);
	transformedSunDirection3 = glm::vec3(transformedSunDirection.x, transformedSunDirection.y, transformedSunDirection.z);
	glUniform3fv(glGetUniformLocation(shaderEnvMap->getShaderId(), "u_directional_light.direction"), 1, (float*)&transformedSunDirection);



	for (std::shared_ptr<Object> object : Game::objects)
	{
		if (object->getType() & ObjectType::Object_Bullet) continue;
		if (!object->getEnabled()) continue;

		if (!(object->getType() & ObjectType::Object_Character))
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


			int isGettingDamagedUniformLocation = GLCALL(glGetUniformLocation(shaderEnvMap->getShaderId(), "u_material.diffuse"));
			if (object->isGettingDamaged()) {

				GLCALL(glUniform1i(isGettingDamagedUniformLocation, 1));
			}
			else {
				GLCALL(glUniform1i(isGettingDamagedUniformLocation, 0));
			}

			GLCALL(glActiveTexture(GL_TEXTURE3));
			GLCALL(glBindTexture(GL_TEXTURE_CUBE_MAP, Map::getSkyboxTexture()));
			GLCALL(glUniform1i(envmapEnvUniformIndex, 3));

			int modelUniformIndex = GLCALL(glGetUniformLocation(shaderEnvMap->getShaderId(), "u_model"));
			GLCALL(glUniformMatrix4fv(modelUniformIndex, 1, GL_FALSE, &model[0][0]));
			int viewUniformIndex = GLCALL(glGetUniformLocation(shaderEnvMap->getShaderId(), "u_view"));
			GLCALL(glUniformMatrix4fv(viewUniformIndex, 1, GL_FALSE, &view[0][0]));
			int projUniformIndex = GLCALL(glGetUniformLocation(shaderEnvMap->getShaderId(), "u_proj"));
			GLCALL(glUniformMatrix4fv(projUniformIndex, 1, GL_FALSE, &proj[0][0]));

			object->renderEnvMap();
		}

	}

	//renderSkybox(glm::mat4(glm::mat3(view)), proj);
	shaderEnvMap->unbind();

	glViewport(0, 0, Renderer::getResolutionX(), Renderer::getResolutionY());
}

void Renderer::drawMapOverlay()
{
	glViewport((Renderer::getResolutionX() - Renderer::getResolutionY()) / 2, 0, Renderer::getResolutionY(), Renderer::getResolutionY());

	float viewsize = 0.55f;
	if (Map::getMapSize().x > Map::getMapSize().y)
	{
		viewsize *= Map::getMapSize().x;
	}
	else {
		viewsize *= Map::getMapSize().y;
	}

	for (std::shared_ptr<Object> object : Game::objects)
	{
		if (!object->getEnabled()) continue;

		if (object->getType() & ObjectType::Object_Character)
		{
			GLCALL(glDisable(GL_CULL_FACE));
			GLCALL(glDisable(GL_DEPTH_TEST));

			if (object->getType() & ObjectType::Object_Player)
			{
				glColor4f(0, 0, 1, 1);
				DrawFilledCircle(object->getPosition().x / viewsize, -object->getPosition().z / viewsize, 0.01, 10);
			}
			else if (object->getType() & ObjectType::Object_NPC)
			{
				glColor4f(1, 0, 0, 1);
				DrawFilledCircle(object->getPosition().x / viewsize, -object->getPosition().z / viewsize, 0.01, 10);
			}
			else if (object->getType() & ObjectType::Object_Character)
			{
				glColor4f(0, 1, 0, 1);
				DrawFilledCircle(object->getPosition().x / viewsize, -object->getPosition().z / viewsize, 0.01, 10);
			}

			GLCALL(glEnable(GL_CULL_FACE));
			GLCALL(glEnable(GL_DEPTH_TEST));
		}
	}

	glViewport(0, 0, Renderer::getResolutionX(), Renderer::getResolutionY());
}

void Renderer::renderEnvironmentMap(std::shared_ptr<Object> objectFromView)
{
	if (objectFromView->getType() & ObjectType::Object_Bullet) return;
	if (objectFromView->getType() & ObjectType::Object_Character) return;

	std::vector<glm::mat4> views;
	glm::mat4 view, proj;

#ifdef DEBUG_ENV_MAP
	Logger::log("Rendering new EnvMap for object: " + objectFromView->printObject());
#endif


	glViewport(0, 0, ConfigManager::env_map_resolution, ConfigManager::env_map_resolution);

	if (objectFromView->getEnvCubeMapFrameBuffer() < 0)
	{
		initFrameBuffer();
	}
	glBindFramebuffer(GL_FRAMEBUFFER, objectFromView->getEnvCubeMapFrameBuffer());
	GLCALL(glActiveTexture(GL_TEXTURE3));
	glBindTexture(GL_TEXTURE_CUBE_MAP, objectFromView->getEnvCubeMap());
	shaderEnvMap->bind();


#pragma region ViewProjection from Object x6

	float angle = 90.0f;
	proj = glm::perspective(glm::radians(angle), 1.0f, 0.1f, 40.0f);

	view = glm::lookAt(objectFromView->getCenter(), objectFromView->getCenter() + glm::vec3(1, 0, 0), glm::vec3(0, -1, 0));
	views.push_back(view);

	view = glm::lookAt(objectFromView->getCenter(), objectFromView->getCenter() + glm::vec3(-1, 0, 0), glm::vec3(0, -1, 0));
	views.push_back(view);

	view = glm::lookAt(objectFromView->getCenter(), objectFromView->getCenter() + glm::vec3(0, 1, 0), glm::vec3(0, 0, 1));
	views.push_back(view);

	view = glm::lookAt(objectFromView->getCenter(), objectFromView->getCenter() + glm::vec3(0, -1, 0), glm::vec3(0, 0, -1));
	views.push_back(view);

	view = glm::lookAt(objectFromView->getCenter(), objectFromView->getCenter() + glm::vec3(0, 0, 1), glm::vec3(0, -1, 0));
	views.push_back(view);

	view = glm::lookAt(objectFromView->getCenter(), objectFromView->getCenter() + glm::vec3(0, 0, -1), glm::vec3(0, -1, 0));
	views.push_back(view);


#pragma endregion



	GLCALL(glCullFace(GL_BACK));
	GLCALL(glClearColor(1, 0, 0, 0.5));

	for (int i = 0; i < 6; i++)
	{
		//Directionallight
		//glm::vec4 transformedSunDirection = glm::vec4(sunDirection, 1.0f);
		glm::vec4 transformedSunDirection = -glm::transpose(glm::inverse(views[i])) * glm::vec4(Map::sun_direction, 1.0f);
		transformedSunDirection3 = glm::vec3(transformedSunDirection.x, transformedSunDirection.y, transformedSunDirection.z);
		glUniform3fv(glGetUniformLocation(shaderEnvMap->getShaderId(), "u_directional_light.direction"), 1, (float*)&transformedSunDirection);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, cube_map_axis1[i], objectFromView->getEnvCubeMap(), 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		for (std::shared_ptr<Object> object : Game::objects)
		{
			if (object->getNumber() == objectFromView->getNumber()) continue;
			if (object->getType() & ObjectType::Object_Character && !ConfigManager::env_map_render_characters) continue;
			if (object->getType() & ObjectType::Object_Bullet) continue;

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


			int isGettingDamagedUniformLocation = GLCALL(glGetUniformLocation(shaderEnvMap->getShaderId(), "u_material.diffuse"));
			if (object->isGettingDamaged()) {

				GLCALL(glUniform1i(isGettingDamagedUniformLocation, 1));
			}
			else {
				GLCALL(glUniform1i(isGettingDamagedUniformLocation, 0));
			}

			GLCALL(glActiveTexture(GL_TEXTURE3));
			GLCALL(glBindTexture(GL_TEXTURE_CUBE_MAP, Map::getSkyboxTexture()));
			GLCALL(glUniform1i(envmapEnvUniformIndex, 3));

			int modelUniformIndex = GLCALL(glGetUniformLocation(shaderEnvMap->getShaderId(), "u_model"));
			GLCALL(glUniformMatrix4fv(modelUniformIndex, 1, GL_FALSE, &model[0][0]));
			int viewUniformIndex = GLCALL(glGetUniformLocation(shaderEnvMap->getShaderId(), "u_view"));
			GLCALL(glUniformMatrix4fv(viewUniformIndex, 1, GL_FALSE, &views[i][0][0]));
			int projUniformIndex = GLCALL(glGetUniformLocation(shaderEnvMap->getShaderId(), "u_proj"));
			GLCALL(glUniformMatrix4fv(projUniformIndex, 1, GL_FALSE, &proj[0][0]));

			object->renderEnvMap();

		}

		renderSkybox(glm::mat4(glm::mat3(views[i])), proj);
		shaderEnvMap->bind();



	}

	// disable
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);




	shaderEnvMap->unbind();
	glViewport(0, 0, Renderer::getResolutionX(), Renderer::getResolutionY()); //actual resolution
	//glViewport(0, 0, Game::getWindowWidth(), Game::getWindowHeight()); //window size
}

void Renderer::renderSkybox(glm::mat4 view, glm::mat4 proj)
{
	shaderSkybox->bind();
	skyboxVertexBuffer->bind();

	glCullFace(GL_BACK);
	glDepthFunc(GL_LEQUAL);

	glUniformMatrix4fv(skyboxViewUniformIndex, 1, GL_FALSE, &view[0][0]);
	glUniformMatrix4fv(skyboxProjUniformIndex, 1, GL_FALSE, &proj[0][0]);

	GLCALL(glActiveTexture(GL_TEXTURE0));
	glBindTexture(GL_TEXTURE_CUBE_MAP, Map::getSkyboxTexture());
	glDrawArrays(GL_TRIANGLES, 0, 36);

	glDepthFunc(GL_LESS);

	skyboxVertexBuffer->unbind();
	shaderSkybox->unbind();
}

/// <summary>
/// can render an image from a given VRAM location Index.
/// Image must first be loaded into the VRAM using the ResourceManager::loadImage() function. The Image can also be from a FrameBuffer
/// also a VertexBuffer is needed, where the image should be rendered
/// </summary>
/// <param name="imageVertexBuffer">position of the image on the screen</param>
/// <param name="imageIndex">VRAM index of the image</param>
void Renderer::renderImage(VertexBuffer* imageVertexBuffer, int imageIndex)
{
	shaderImage->bind();
	imageVertexBuffer->bind();

	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 proj = glm::mat4(1.0f);

	GLCALL(glUniformMatrix4fv(glGetUniformLocation(shaderImage->getShaderId(), "u_model"), 1, GL_FALSE, &model[0][0]));
	GLCALL(glUniformMatrix4fv(glGetUniformLocation(shaderImage->getShaderId(), "u_view"), 1, GL_FALSE, &view[0][0]));
	GLCALL(glUniformMatrix4fv(glGetUniformLocation(shaderImage->getShaderId(), "u_proj"), 1, GL_FALSE, &proj[0][0]));

	GLCALL(glDisable(GL_CULL_FACE));
	GLCALL(glDisable(GL_DEPTH_TEST));

	GLCALL(glActiveTexture(GL_TEXTURE0));
	GLCALL(glBindTexture(GL_TEXTURE_2D, imageIndex));
	GLCALL(glDrawArrays(GL_TRIANGLES, 0, 6));

	GLCALL(glEnable(GL_CULL_FACE));
	GLCALL(glEnable(GL_DEPTH_TEST));


	imageVertexBuffer->unbind();
	shaderImage->unbind();
}

void Renderer::renderImage(VertexBuffer* imageVertexBuffer, glm::vec3 position, glm::vec3 rotation, int imageIndex, glm::vec3 scale)
{
	shaderImage->bind();
	imageVertexBuffer->bind();

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::scale(model, scale);

	//move to position of model
	model = glm::translate(model, position);

	//rotate model around X
	float angle = rotation.x;
	model = glm::rotate(model, glm::radians(angle), glm::vec3(1, 0, 0));

	//rotate model around Y
	angle = rotation.y;
	model = glm::rotate(model, glm::radians(angle), glm::vec3(0, 1, 0));

	//rotate model around z
	angle = rotation.z;
	model = glm::rotate(model, glm::radians(angle), glm::vec3(0, 0, 1));

	//view and projection
	modelViewProj = Game::players[0]->getViewProj() * model;
	glm::mat4 modelView = Game::players[0]->getView() * model;
	glm::mat4 invModelView = glm::transpose(glm::inverse(modelView));

	glm::mat4 view = Game::players[0]->getView();
	glm::mat4 proj = Game::players[0]->getProj();

	GLCALL(glUniformMatrix4fv(glGetUniformLocation(shaderImage->getShaderId(), "u_model"), 1, GL_FALSE, &model[0][0]));
	GLCALL(glUniformMatrix4fv(glGetUniformLocation(shaderImage->getShaderId(), "u_view"), 1, GL_FALSE, &view[0][0]));
	GLCALL(glUniformMatrix4fv(glGetUniformLocation(shaderImage->getShaderId(), "u_proj"), 1, GL_FALSE, &proj[0][0]));

	GLCALL(glDisable(GL_CULL_FACE));
	//GLCALL(glDisable(GL_DEPTH_TEST));

	GLCALL(glActiveTexture(GL_TEXTURE0));
	GLCALL(glBindTexture(GL_TEXTURE_2D, imageIndex));
	GLCALL(glDrawArrays(GL_TRIANGLES, 0, 6));

	GLCALL(glEnable(GL_CULL_FACE));
	//GLCALL(glEnable(GL_DEPTH_TEST));


	imageVertexBuffer->unbind();
	shaderImage->unbind();
}

void Renderer::renderObjects(bool transparent)
{
	GLCALL(glCullFace(GL_BACK));

	shaderBasic->bind();
	int cameraposUniformIndex = GLCALL(glGetUniformLocation(shaderBasic->getShaderId(), "u_camerapos"));
	GLCALL(glUniform3fv(cameraposUniformIndex, 1, (float*)&Game::players[0]->getCameraPosition()));


	for (std::shared_ptr<Object> object : Game::objects)
	{
		if (!object->getEnabled()) continue;
		if (transparent && !object->getModel()->getHasTransparentTexture()) continue;
		if (!transparent && object->getModel()->getHasTransparentTexture()) continue;
		if (object->getType() & ObjectType::Object_Player) continue;


		if (ConfigManager::env_map_render_interval != 0)
		{
			if (frameCount % ConfigManager::env_map_render_interval == 0)		//render the env map every couple frame again
			{
				renderEnvironmentMap(object);
			}
		}
		else {
			if (frameCount == 1)		//render the env map just once at startup
			{
				renderEnvironmentMap(object);
			}
		}

		frameBuffer.bind();
		object->bindShader();

		glViewport(0, 0, Renderer::getResolutionX(), Renderer::getResolutionY()); //render in the desired resolution

		GLCALL(glActiveTexture(GL_TEXTURE3));
		if (object->getEnvCubeMap() >= 0 && !(object->getType() & ObjectType::Object_Character))
		{
			GLCALL(glBindTexture(GL_TEXTURE_CUBE_MAP, object->getEnvCubeMap()));
		}
		else
		{
			GLCALL(glBindTexture(GL_TEXTURE_CUBE_MAP, Map::getSkyboxTexture()));
		}
		GLCALL(glUniform1i(envmapBasicUniformIndex, 3));


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

		int markcolorUniformLocation = GLCALL(glGetUniformLocation(shaderBasic->getShaderId(), "u_mark_color"));
		glm::vec3 markColor = glm::vec3(0,0,0);
		if (object->isMarked()) {
			markColor = object->getMarkColor();
		}
		GLCALL(glUniform3fv(markcolorUniformLocation, 1, (float*) &markColor));


		int modelUniformLocation = GLCALL(glGetUniformLocation(shaderBasic->getShaderId(), "u_model"));
		GLCALL(glUniformMatrix4fv(modelUniformLocation, 1, GL_FALSE, &model[0][0]));


		GLCALL(glUniformMatrix4fv(modelUniformIndex, 1, GL_FALSE, &model[0][0]));
		GLCALL(glUniformMatrix4fv(viewUniformIndex, 1, GL_FALSE, &view[0][0]));
		GLCALL(glUniformMatrix4fv(projUniformIndex, 1, GL_FALSE, &proj[0][0]));

		object->render();

		object->unbindShader();
	}

}

void Renderer::renderTransparentObjects()
{
	renderObjects(true);
}

void Renderer::renderOpaqueObjects()
{
	renderObjects(false);
}

void Renderer::renderAxis(glm::vec3 vector, int x, int y)
{
	shaderGeometry->bind();
	axisVertexBuffer->bind();

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
	case ShaderType::envmap:
		return shaderEnvMap;
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
	glViewport(0, 0, Game::getWindowWidth(), Game::getWindowHeight()); //window size

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

	if (Game::getGameState() == GameState::GAME_MENU || Game::getGameState() == GameState::GAME_PAUSED)
	{
		GLCALL(glUniform1i(glGetUniformLocation(shaderPostProcessing->getShaderId(), "u_postprocessingeffect.menu"), 1));
	}
	else
	{
		GLCALL(glUniform1i(glGetUniformLocation(shaderPostProcessing->getShaderId(), "u_postprocessingeffect.menu"), 0));
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
	GLCALL(glClearColor(0, 0, 0, 0));
	GLCALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

	frameCount++;
}

void Renderer::applyPostprocessingEffect(PostProcessingEffect postprocessingeffect, float32 duration)
{
	postprocessingEffectDuration[postprocessingeffect] = duration;
}

void Renderer::toggleShadows(ShadowOption option)
{
	ConfigManager::shadow_option = option;
	shaderBasic->bind();

	switch (option)
	{
	case ShadowOption::off:
		GLCALL(glUniform1i(shadowmodeUniformIndex, 0));
		break;
	case ShadowOption::hard:
		GLCALL(glUniform1i(shadowmodeUniformIndex, 1));
		break;
	case ShadowOption::soft:
		GLCALL(glUniform1i(shadowmodeUniformIndex, 2));
		break;
	}
}

void Renderer::changeResolution(int x, int y)
{
	if (ConfigManager::fullscreen_option == FullscreenOption::fullscreen)
	{
		if (x != 0)
			ConfigManager::fullscreen_resolution_width = x;
		if (y != 0)
			ConfigManager::fullscreen_resolution_height = y;
	}
	else
	{
		if (x != 0)
			ConfigManager::windowed_resolution_width = x;
		if (y != 0)
			ConfigManager::windowed_resolution_height = y;
	}

	initFrameBuffer();
}