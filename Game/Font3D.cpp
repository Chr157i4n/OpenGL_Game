#include "Font3D.h"

#include "UI.h"
#include "Renderer.h"
#include "Game.h"


float fontVertices[] = {
	// positions          
	-1.0f,	-0.2f,	0.0f,	0.0f, 0.0f,
	-1.0f,	+0.2f,	0.0f,	0.0f, 1.0f,
	1.0f,	-0.2f,	0.0f,	1.0f, 0.0f,

	1.0f,	-0.2f,	0.0f,	1.0f, 0.0f,
	-1.0f,	+0.2f,	0.0f,	0.0f, 1.0f,
	1.0f,   0.2f,	0.0f,	1.0f, 1.0f,
};


Font3D::Font3D()
{
	UI3D.create(1000, 200, FrameBufferTextureType::colorMap | FrameBufferTextureType::stencilMap);

	fontVertexBuffer = new VertexBuffer(fontVertices, 6, VertexType::_VertexPosTex);
}

void Font3D::createTexture()
{
	glViewport(0, 0, 8000 * scale, 1600 * scale);

	UI3D.bind();
	GLCALL(glClearColor(0, 0, 0, 0));
	GLCALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

	GLCALL(glDisable(GL_CULL_FACE));

	UI::drawString(10, 10, text, color);

	GLCALL(glEnable(GL_CULL_FACE));

	UI3D.unbind();

	//actual resolution
	glViewport(0, 0, Game::getWindowWidth(), Game::getWindowHeight());
}

void Font3D::renderTexture()
{
	glm::vec3 pos, rot;

	if (object_related == nullptr)
		pos = position;
	else
		pos = object_related->getPosition() + position;

	if (rotation == glm::vec3(-1))
		rot = Game::players[0]->getRotationInv();
	else
		rot = rotation;

	Renderer::renderImage(fontVertexBuffer, pos, rot, *UI3D.getTextureId());
}
