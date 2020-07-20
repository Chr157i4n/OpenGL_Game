#pragma once

#include <GL/glew.h>
#include <SDL.h>

#include "Shader.h"
#include "ResourceManager.h"
#include "VertexBuffer.h"
#include "FrameBuffer.h"
#include "UI.h"

enum ShaderType {
	basic,
	skybox,
};

static class Renderer
{
public:
	static void initOpenGL(SDL_Window** window);

	static void loadShader();

	static void init(std::shared_ptr<Player> player);


	static void showLoadingScreen();


	static void calcLight(std::shared_ptr<Player> player);

	static void calcShadows(std::vector< std::shared_ptr<Object>> objects);

	static void renderShadowsMap(std::vector< std::shared_ptr<Object>> objects);

	static void showShadowMap();

	static void renderSkybox(std::shared_ptr<Player> player);

	static void renderImage(VertexBuffer* imageVertexBuffer, int imageIndex);

	static void renderObjects(std::shared_ptr<Player> player, std::vector< std::shared_ptr<Object>> objects);

	static void renderAxis(glm::vec3 vector, int x, int y);

	static void setModels(std::vector<Model*> newModels);

	static std::vector<Model*> getModels();

	static Shader* getShader(ShaderType shadertype);

	static void toggleWireframe();

	static void toggleShowNormals();

	static void postProcessing();

	static void clearBuffer();


private:
	static void initLight();

	static SDL_Window** window;
	
	static Shader* shaderSkybox;
	static Shader* shaderBasic;
	static Shader* shaderImage;
	static Shader* shaderGeometry;
	static Shader* shaderPostProcessing;
	static Shader* shaderDepthMap;

	static unsigned int loadingScreenTexture;

	static unsigned int skyboxTexture;
	static VertexBuffer* skyboxVertexBuffer;
	static VertexBuffer* axisVertexBuffer;
	static VertexBuffer* screenVertexBuffer;

	

	static std::vector<Model*> models;

	static int modelUniformIndex;
	static int viewUniformIndex;
	static int projUniformIndex;
	static int skyboxViewProjectionUniformIndex;
	static int lightdirectionUniformIndex;
	static int lightpositionUniformIndex;

	static glm::mat4 modelViewProj;


	static glm::vec3 sunDirection;
	static glm::vec3 spotLightPosition;
	static glm::vec4 pointLightPosition;

	static bool wireframeMode;
	static bool showNormalMode;

	static const int shadowMapResolution = 4096;

	

public:
	static FrameBuffer frameBuffer;
	static FrameBuffer depthMapBuffer;

	static glm::vec3 transformedSunDirection3;
	

	
};

