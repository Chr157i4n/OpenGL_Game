#pragma once

#include "FrameBuffer.h"
#include "VertexBuffer.h"
#include <string>
#include <memory>

#include "Object.h"

class Font3D
{
	FrameBuffer UI3D;
	VertexBuffer *fontVertexBuffer;

public:

	glm::vec3 position = glm::vec3(-1);
	glm::vec3 rotation = glm::vec3(-1);
	std::string text = "";
	glm::vec4 color = glm::vec4(0, 0, 0, 1);
	float scale = 1;

	std::shared_ptr<Object> object_related = nullptr;

	Font3D();

	void createTexture();

	void renderTexture();

	~Font3D()
	{
		UI3D.destroy();
	}


};

