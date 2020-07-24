#pragma once
#include "defines.h"

#include <GL/glew.h>
#include <SDL.h>

#include "FrameBuffer.h"
#include "Shader.h"
#include "VertexBuffer.h"
#include "Model.h"
#include "UI_Element_ProgressBar.h"

enum ShaderType {
	basic,
	skybox,
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

	static void showLoadingScreen();


	static void calcLight();

	static void calcShadows();

	static void renderShadowsMap();

	static void showShadowMap();

	static void renderSkybox();

	static void renderImage(VertexBuffer* imageVertexBuffer, int imageIndex);

	static void renderObjects();

	static void renderAxis(glm::vec3 vector, int x, int y);

	static void setModels(std::vector<Model*> newModels);

	static std::vector<Model*> getModels();

	static Shader* getShader(ShaderType shadertype);

	static void toggleWireframe();

	static void toggleShowNormals();

	static void postProcessing();

	static void clearBuffer();

	static void applyPostprocessingEffect(PostProcessingEffect postprocessingeffect, float32 duration);


private:
	static void initLight();
	
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

	static int shadowMapResolution;


public:
	static FrameBuffer frameBuffer;
	static FrameBuffer depthMapBuffer;

	static glm::vec3 transformedSunDirection3;

	static UI_Element_ProgressBar* loadingProgressBar;

	//if this value is above 0 the effect will be applied
	static std::vector<float32>postprocessingEffectDuration;
	

	
};

