#include "Renderer.h"


#include "libs/stb_image.h"


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

Shader* Renderer::skyboxShader = nullptr;
unsigned int Renderer::cubemapTexture;
VertexBuffer* Renderer::skyboxVertexBuffer;
IndexBuffer* Renderer::skyboxIndexBuffer;

int Renderer::skyboxViewProjectionUniformIndex;

void Renderer::init()
{
	skyboxShader = ResourceManager::loadShader("shaders\\skybox.vert", "shaders\\skybox.frag");

	skyboxViewProjectionUniformIndex = GLCALL(glGetUniformLocation(skyboxShader->getShaderId(), "u_viewprojection"));

	skyboxVertexBuffer = new VertexBuffer(skyboxVertices, 36, 1);

	std::vector<std::string> faces
	{
			"textures\\skybox\\skybox1_right.jpg",
			"textures\\skybox\\skybox1_left.jpg",
			"textures\\skybox\\skybox1_top.jpg",
			"textures\\skybox\\skybox1_bottom.jpg",
			"textures\\skybox\\skybox1_front.jpg",
			"textures\\skybox\\skybox1_back.jpg"
	};
	cubemapTexture = loadCubemap(faces);
}

void Renderer::renderSkybox(Player* player)
{
	skyboxShader->bind();
	skyboxVertexBuffer->bind();

	glDepthMask(GL_FALSE);
	// ... set view and projection matrix
	glm::mat4 proj = player->getViewProj();

	glUniformMatrix4fv(skyboxViewProjectionUniformIndex, 1, GL_FALSE, &proj[0][0]);
	
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glDepthMask(GL_TRUE);

	skyboxVertexBuffer->unbind();
	skyboxShader->unbind();
}

int Renderer::loadCubemap(std::vector<std::string> faces)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	stbi_set_flip_vertically_on_load(false);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
			);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}
