#pragma once
#include "defines.h"

#include <GL/glew.h>
#include <SDL.h>
#include "libs/glm/glm.hpp"
#include "libs/glm/ext/matrix_transform.hpp"
#include "libs/glm/gtc/matrix_transform.hpp"

#include "FrameBuffer.h"
#include "Shader.h"
#include "VertexBuffer.h"
#include "Model.h"
#include "UI_Element_ProgressBar.h"
#include "Object.h"
#include "ConfigManager.h"

#define NR_POINT_LIGHTS 10

enum ShaderType {
	basic,
	skybox,
	envmap,
};

enum PostProcessingEffect {
	negative,
	blood,
	uncolored,
};


static class Renderer
{
public:
	static void initOpenGL();

	static void loadShader();

	static void init();

	static void initFrameBuffer();

	static void initLoadingScreen();

	static void drawLoadingScreen();

	static void drawMainMenuBackground();


	static void initLight();

	static void calcLight();

	static void calcShadows();

	static void renderShadowsMap();

	static void showShadowMap();

	static void DrawCircle(float cx, float cy, float r, int num_segments);

	static void DrawFilledCircle(float cx, float cy, float r, int num_segments);

	static void render3DString(glm::vec3 position, glm::vec3 rotation, std::string text, glm::vec4 color);

	static void renderMap();

	static void drawMapOverlay();

	static void renderEnvironmentMap(std::shared_ptr<Object> objectFromView);

	static void renderSkybox(glm::mat4 view, glm::mat4 proj);

	static void renderImage(VertexBuffer* imageVertexBuffer, int imageIndex);

	static void renderImage(VertexBuffer* imageVertexBuffer, glm::vec3 position, glm::vec3 rotation, int imageIndex, glm::vec3 scale=glm::vec3(1));

	static void renderObjects(bool transparent = false);

	static void renderTransparentObjects();

	static void renderOpaqueObjects();

	static void renderAxis(glm::vec3 vector, int x, int y);

	static void setModels(std::vector<Model*> newModels);

	static std::vector<Model*> getModels();

	static Shader* getShader(ShaderType shadertype);

	static void toggleWireframe();

	static void toggleShowNormals();

	static void postProcessing();

	static void clearBuffer();

	static void applyPostprocessingEffect(PostProcessingEffect postprocessingeffect, float32 duration);

	static void resetFrameCount()
	{
		frameCount = 0;
	}

	static void enableVSync(bool enable)
	{
		SDL_GL_SetSwapInterval(enable);
	}

	static void toggleVSync()
	{
		if (SDL_GL_GetSwapInterval() == 0)
		{
			SDL_GL_SetSwapInterval(1);
			ConfigManager::v_sync = true;
		}
		else
		{
			SDL_GL_SetSwapInterval(0);
			ConfigManager::v_sync = false;
		}
	}

	static void toggleVSync(int enable)
	{
		SDL_GL_SetSwapInterval(enable);
		ConfigManager::v_sync = enable;
	}

	static void toggleShadows(ShadowOption option);

	static void changeResolution(int x, int y);

	static int getResolutionX()
	{
		if (ConfigManager::fullscreen_option == FullscreenOption::fullscreen)
			return ConfigManager::fullscreen_resolution_width;
		else
			return ConfigManager::windowed_resolution_width;
	}

	static int getResolutionY()
	{
		if (ConfigManager::fullscreen_option == FullscreenOption::fullscreen)
			return ConfigManager::fullscreen_resolution_height;
		else
			return ConfigManager::windowed_resolution_height;
	}

	static int getFrameCount()
	{
		return frameCount;
	}

	static FrameBuffer frameBuffer;
	static FrameBuffer shadowMapBuffer;
	static FrameBuffer envMapFacesBuffer[6];
	static FrameBuffer envMapBuffer;

	static glm::vec3 transformedSunDirection3;

	static UI_Element_ProgressBar* loadingProgressBar;


private:
	
	static std::vector<float32>postprocessingEffectDuration; //if this value is above 0 the effect will be applied

	static Shader* shaderSkybox;
	static Shader* shaderBasic;
	static Shader* shaderImage;
	static Shader* shaderGeometry;
	static Shader* shaderPostProcessing;
	static Shader* shaderShadowMap;
	static Shader* shaderEnvMap;

	static unsigned int loadingScreenTexture;
	static unsigned int main_menu_backgroundTexture;
	static VertexBuffer* skyboxVertexBuffer;
	static VertexBuffer* axisVertexBuffer;
	static VertexBuffer* screenVertexBuffer;

	

	static std::vector<Model*> models;

	static int modelUniformIndex;
	static int viewUniformIndex;
	static int projUniformIndex;
	static int skyboxViewUniformIndex;
	static int skyboxProjUniformIndex;
	static int lightdirectionBasicUniformIndex;
	static int lightdirectionEnvUniformIndex;
	static int envmapBasicUniformIndex;
	static int envmapEnvUniformIndex;
	static int lightspacematrixUniformIndex;
	static int shadowmapUniformIndex;
	static int shadowmodeUniformIndex;
		

	static glm::mat4 modelViewProj;

	static glm::vec3 spotLightPosition;

	static bool wireframeMode;
	static bool showNormalMode;

	static int frameCount;
	
};

