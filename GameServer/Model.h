#pragma once
#include "defines.h"

#include <string>
#include <vector>

#include <glm/glm.hpp>

/// <summary>
/// Server-side model metadata. The dedicated server does not render meshes,
/// it only needs dimensions for collision and the BMF parser has been replaced
/// by an Assimp based geometry loader.
/// </summary>
class Model
{
public:

	bool init(const char* filename);

	glm::vec3 getDimension() const
	{
		return dimension;
	}

	glm::vec3 getBoundingBoxDimension() const
	{
		return boundingBoxDimension;
	}

	bool getHasTransparentTexture() const
	{
		return hasTransparentTexture;
	}

	void setModelName(std::string newModelName)
	{
		modelName = newModelName;
	}

	std::string getModelName() const
	{
		return modelName;
	}

	void setModelID(int newModelID)
	{
		modelID = newModelID;
	}

	int getModelID() const
	{
		return modelID;
	}

private:

	glm::vec3 dimension = glm::vec3(2, 2, 2);
	glm::vec3 boundingBoxDimension = glm::vec3(4, 4, 4);

	std::string modelName = "";
	int modelID = -1;

	bool hasTransparentTexture = false;
};