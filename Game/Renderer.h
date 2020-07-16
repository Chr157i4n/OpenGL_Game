#pragma once

#include "Shader.h"
#include "ResourceManager.h"
#include "VertexBuffer.h"



static class Renderer
{
public:
	static void init();

	static void renderSkybox(Player* player);

	static int loadCubemap(std::vector<std::string> faces);

private:
	
	static Shader* skyboxShader;
	static unsigned int cubemapTexture;
	static VertexBuffer* skyboxVertexBuffer;
	static IndexBuffer* skyboxIndexBuffer;

	static int skyboxViewProjectionUniformIndex;
};

