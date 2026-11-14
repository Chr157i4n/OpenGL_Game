#pragma once
#include "defines.h"
#include "Types.h"
#include "ObjectCore.h"

#include <algorithm>
#include <chrono>
#include <limits>
#include <memory>
#include <string>
#include <vector>

#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include "Shader.h"
#include "Model.h"

// forward declaration
struct Ray;

struct Box {
	float minX, maxX, minY, maxY, minZ, maxZ;
};

/// <summary>
/// Client object: rendering, shader, env maps and interaction behavior.
/// Simulation logic lives in ObjectCore.
/// </summary>
class Object : public ObjectCore
{
public:
	Object(Shader* shader, std::string modelFileName);

	Shader* getShader();

	void bindShader();
	void unbindShader();

	bool intersectWithRay(Ray ray) const;

	float getDistance(std::shared_ptr<Object> object) const;
	float getDistance(glm::vec3 location) const;

	virtual void interact_click();
	virtual void interact_hold();

	CollisionResult checkCollision();

	void reactToCollision(CollisionResult collisionResult);

	bool checkBoundaries();

	void fall();
	void move();

	void setModel(Model* newModel);
	void setModel(std::string newModelFileName);
	Model* getModel() const;

	void render();
	void renderShadowMap();
	void renderEnvMap();

	Box getOrientedBoundingBox() const
	{
		return orientedBoundingBox;
	}

	void setEnvCubeMap(unsigned int newEnvCubeMap)
	{
		envCubeMap = static_cast<int>(newEnvCubeMap);
	}

	int getEnvCubeMap() const
	{
		return envCubeMap;
	}

	void setEnvCubeMapFrameBuffer(unsigned int newEnvCubeMapFrameBuffer)
	{
		envCubeMapFrameBuffer = static_cast<int>(newEnvCubeMapFrameBuffer);
	}

	int getEnvCubeMapFrameBuffer() const
	{
		return envCubeMapFrameBuffer;
	}

	void setEnvCubeMapDepthBuffer(unsigned int newEnvCubeMapDepthBuffer)
	{
		envCubeMapDepthBuffer = static_cast<int>(newEnvCubeMapDepthBuffer);
	}

	int getEnvCubeMapDepthBuffer() const
	{
		return envCubeMapDepthBuffer;
	}

	void setTextureFlow(glm::vec2 newTextureFlow)
	{
		textureFlow = newTextureFlow;
	}

	glm::vec2 getTextureFlow() const
	{
		return textureFlow;
	}

	glm::vec3 getMarkColor() const
	{
		return markColor;
	}

protected:
	virtual void updateCollisionGeometry() override;

	Shader* shader = nullptr;
	Model* model = nullptr;
	int modelIndex = -1;

	Box orientedBoundingBox{};
	glm::vec2 textureFlow = glm::vec2(0, 0);
	glm::vec3 markColor = glm::vec3(0.1f, 0.1f, 0.0f);

	int envCubeMap = -1;
	int envCubeMapFrameBuffer = -1;
	int envCubeMapDepthBuffer = -1;
};
