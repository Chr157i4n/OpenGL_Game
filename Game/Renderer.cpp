#include "Renderer.h"


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
	-400.0f,  400.0f, -400.0f,
	-400.0f, -400.0f, -400.0f,
	 400.0f, -400.0f, -400.0f,
	 400.0f, -400.0f, -400.0f,
	 400.0f,  400.0f, -400.0f,
	-400.0f,  400.0f, -400.0f,

	-400.0f, -400.0f,  400.0f,
	-400.0f, -400.0f, -400.0f,
	-400.0f,  400.0f, -400.0f,
	-400.0f,  400.0f, -400.0f,
	-400.0f,  400.0f,  400.0f,
	-400.0f, -400.0f,  400.0f,

	 400.0f, -400.0f, -400.0f,
	 400.0f, -400.0f,  400.0f,
	 400.0f,  400.0f,  400.0f,
	 400.0f,  400.0f,  400.0f,
	 400.0f,  400.0f, -400.0f,
	 400.0f, -400.0f, -400.0f,

	-400.0f, -400.0f,  400.0f,
	-400.0f,  400.0f,  400.0f,
	 400.0f,  400.0f,  400.0f,
	 400.0f,  400.0f,  400.0f,
	 400.0f, -400.0f,  400.0f,
	-400.0f, -400.0f,  400.0f,

	-400.0f,  400.0f, -400.0f,
	 400.0f,  400.0f, -400.0f,
	 400.0f,  400.0f,  400.0f,
	 400.0f,  400.0f,  400.0f,
	-400.0f,  400.0f,  400.0f,
	-400.0f,  400.0f, -400.0f,

	-400.0f, -400.0f, -400.0f,
	-400.0f, -400.0f,  400.0f,
	 400.0f, -400.0f, -400.0f,
	 400.0f, -400.0f, -400.0f,
	-400.0f, -400.0f,  400.0f,
	 400.0f, -400.0f,  400.0f
};

SDL_Window** Renderer::window;

Shader* Renderer::shaderSkybox = nullptr;
Shader* Renderer::shaderBasic = nullptr;
Shader* Renderer::shaderImage = nullptr;

unsigned int Renderer::loadingScreenTexture;

unsigned int Renderer::skyboxTexture;
VertexBuffer* Renderer::skyboxVertexBuffer;
IndexBuffer* Renderer::skyboxIndexBuffer;

std::vector<Model*> Renderer::models;

int Renderer::skyboxViewProjectionUniformIndex;
int Renderer::modelViewProjMatrixUniformIndex;
int Renderer::modelViewUniformIndex;
int Renderer::invmodelViewUniformIndex;
int Renderer::lightdirectionUniformIndex;
int Renderer::lightpositionUniformIndex;

glm::vec3 Renderer::sunDirection;
glm::vec3 Renderer::spotLightPosition;
glm::vec4 Renderer::pointLightPosition;

glm::mat4 Renderer::modelViewProj;


void Renderer::initOpenGL(SDL_Window** window)
{
	Renderer::window = window;

	SDL_Init(SDL_INIT_EVERYTHING);

	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetSwapInterval(1);

	/*SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 8);*/

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

	*window = SDL_CreateWindow("OpenGL-Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, resolution_width, resolution_height, flags);
	SDL_GLContext glContext = SDL_GL_CreateContext(*window);

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
}

void Renderer::initShader()
{
	shaderSkybox = ResourceManager::loadShader("shaders/skybox.vert", "shaders/skybox.frag");
	shaderBasic = ResourceManager::loadShader("shaders/basic.vert", "shaders/basic.frag");
	shaderImage = ResourceManager::loadShader("shaders/image.vert", "shaders/image.frag");
}

void Renderer::init(Player* player)
{
	skyboxVertexBuffer = new VertexBuffer(skyboxVertices, 36, 1);

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
	skyboxViewProjectionUniformIndex = GLCALL(glGetUniformLocation(shaderSkybox->getShaderId(), "u_viewprojection"));
	modelViewProjMatrixUniformIndex = GLCALL(glGetUniformLocation(shaderBasic->getShaderId(), "u_modelViewProj"));
	modelViewUniformIndex = GLCALL(glGetUniformLocation(shaderBasic->getShaderId(), "u_modelView"));
	invmodelViewUniformIndex = GLCALL(glGetUniformLocation(shaderBasic->getShaderId(), "u_invModelView"));

	initLight();
}

void Renderer::initLight()
{
	shaderBasic->bind();

	lightdirectionUniformIndex = GLCALL(glGetUniformLocation(shaderBasic->getShaderId(), "u_directional_light.direction"));
	//glm::vec3 sunColor = glm::vec3(0.98f, 0.83f, 0.30f);
	glm::vec3 sunColor = glm::vec3(0.5);
	sunDirection = glm::vec3(0.8, -0.4, -0.4);
	GLCALL(glUniform3fv(glGetUniformLocation(shaderBasic->getShaderId(), "u_directional_light.diffuse"), 1, (float*)&sunColor));
	GLCALL(glUniform3fv(glGetUniformLocation(shaderBasic->getShaderId(), "u_directional_light.specular"), 1, (float*)&sunColor));
	sunColor = glm::vec3(0.5);
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
}


void Renderer::showLoadingScreen() {
	
	GLCALL(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
	GLCALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));


	VertexBuffer* loadingScreenVertexBuffer = new VertexBuffer(screenVertices, 6, 2);
	loadingScreenTexture = ResourceManager::loadImage("images/loading_screen.png");
	
	renderImage(loadingScreenVertexBuffer, loadingScreenTexture);

	SDL_GL_SwapWindow(*window);

	delete loadingScreenVertexBuffer;
}


void Renderer::calcLight(Player* player)
{
	glm::vec4 transformedSunDirection = glm::transpose(glm::inverse(player->getView())) * glm::vec4(sunDirection, 1.0f);
	glUniform3fv(lightdirectionUniformIndex, 1, (float*)&transformedSunDirection);

	glm::mat4 pointLightMatrix = glm::mat4(1.0f);
	pointLightPosition = pointLightMatrix * pointLightPosition;
	glm::vec3 transformedPointLightPosition = (glm::vec3) (player->getView() * pointLightPosition);
	glUniform3fv(lightpositionUniformIndex, 1, (float*)&transformedPointLightPosition);
}

void Renderer::renderSkybox(Player* player)
{
	shaderSkybox->bind();
	skyboxVertexBuffer->bind();

	glDepthMask(GL_FALSE);
	// ... set view and projection matrix
	glm::mat4 viewproj = player->getViewProj();

	glUniformMatrix4fv(skyboxViewProjectionUniformIndex, 1, GL_FALSE, &viewproj[0][0]);

	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glDepthMask(GL_TRUE);

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

	glBindTexture(GL_TEXTURE_2D, imageIndex);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDepthMask(GL_TRUE);
	GLCALL(glEnable(GL_CULL_FACE));
	GLCALL(glEnable(GL_DEPTH_TEST));


	imageVertexBuffer->unbind();
	shaderImage->unbind();
}

void Renderer::renderObjects(Player* player, std::vector<Object*> objects)
{
	for (Object* object : objects)
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
		modelViewProj = player->getViewProj() * model;
		glm::mat4 modelView = player->getView() * model;
		glm::mat4 invModelView = glm::transpose(glm::inverse(modelView));

		object->bindShader();

		GLCALL(glUniformMatrix4fv(modelViewProjMatrixUniformIndex, 1, GL_FALSE, &modelViewProj[0][0]));
		GLCALL(glUniformMatrix4fv(modelViewUniformIndex, 1, GL_FALSE, &modelView[0][0]));
		GLCALL(glUniformMatrix4fv(invmodelViewUniformIndex, 1, GL_FALSE, &invModelView[0][0]));

		object->render();
	}
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


