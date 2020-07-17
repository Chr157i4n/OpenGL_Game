#pragma once

#include <GL/glew.h>
#include <SDL.h>

#include "Shader.h"
#include "ResourceManager.h"
#include "VertexBuffer.h"
#include "UI.h"

enum ShaderType {
	basic,
	skybox,
};

static class Renderer
{
public:
	static void initOpenGL(SDL_Window** window);

	static void initShader();

	static void init(Player* player);


	static void showLoadingScreen();


	static void calcLight(Player* player);

	static void renderSkybox(Player* player);

	static void renderImage(VertexBuffer* imageVertexBuffer, int imageIndex);

	static void renderObjects(Player* player, std::vector<Object*> objects);

	static void setModels(std::vector<Model*> newModels);

	static std::vector<Model*> getModels();

	static Shader* getShader(ShaderType shadertype);

private:
	static void initLight();

	static SDL_Window** window;
	
	static Shader* shaderSkybox;
	static Shader* shaderBasic;
	static Shader* shaderImage;

	static unsigned int loadingScreenTexture;

	static unsigned int skyboxTexture;
	static VertexBuffer* skyboxVertexBuffer;
	static IndexBuffer* skyboxIndexBuffer;

	

	static std::vector<Model*> models;

	static int modelViewProjMatrixUniformIndex;
	static int modelViewUniformIndex;
	static int invmodelViewUniformIndex;
	static int skyboxViewProjectionUniformIndex;
	static int lightdirectionUniformIndex;
	static int lightpositionUniformIndex;

	static glm::mat4 modelViewProj;


	static glm::vec3 sunDirection;
	static glm::vec3 spotLightPosition;
	static glm::vec4 pointLightPosition;
	

	
};

