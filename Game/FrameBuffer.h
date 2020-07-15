#pragma once
#include <GL/glew.h>

#include "defines.h"

struct FrameBuffer {
    void create(uint32 width, uint32 height);

    void destroy();

    void bind();

    void unbind();

private:
    GLuint fbo;
    GLuint textures[2];
};

