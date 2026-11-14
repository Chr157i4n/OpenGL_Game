#pragma once
#include "ObjectCore.h"

#include "Model.h"

/// <summary>
/// Server object: shared ObjectCore simulation plus model loading.
/// </summary>
class Object : public ObjectCore
{
public:

	explicit Object(std::string modelFileName);

	CollisionResult checkCollision();

	void reactToCollision(CollisionResult collisionResult);

	bool checkBoundaries();

	void fall();

	void move();

	float getDistance(std::shared_ptr<Object> object) const;

	void setModel(Model* newModel);

	void setModel(std::string newModelFileName);

	Model* getModel() const;

protected:

	Model* model = nullptr;

	int modelIndex = -1;

	glm::vec2 textureFlow = glm::vec2(0, 0);

	int envCubeMap = -1;
	int envCubeMapFrameBuffer = -1;
};