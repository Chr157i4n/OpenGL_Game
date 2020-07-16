#pragma once

#include "defines.h"
#include <GL/glew.h>
#include <stddef.h>
#include <vector>


struct VertexBuffer {
    VertexBuffer(void* data, uint32 numVertices, int numElements);

    virtual ~VertexBuffer();

    void bind();

    void unbind();

private:
    GLuint bufferId;
    GLuint vao;
};

