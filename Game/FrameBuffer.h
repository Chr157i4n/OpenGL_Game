#pragma once
#include "defines.h"

#include <GL/glew.h>


enum FrameBufferTextureType {
    colorMap    = 1,
    stencilMap  = 2,
    depthMap    = 4,
    envMap      = 8,
    envMapFace = 16,
};

inline FrameBufferTextureType operator|(FrameBufferTextureType a, FrameBufferTextureType b)
{
    return static_cast<FrameBufferTextureType>(static_cast<int>(a) | static_cast<int>(b));
}

struct FrameBuffer {
    void create(uint32 width, uint32 height, FrameBufferTextureType textureTypeFlags);

    void destroy();

    void bind();

    void unbind();

    GLuint* getTextureId() {
        return textures;
    }

private:
    GLuint fbo;
    GLuint textures[16];
};

