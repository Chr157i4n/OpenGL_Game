#pragma once
#include "defines.h"

#include <GL/glew.h>
#include <stddef.h>
#include <vector>

enum VertexType {
    _Vertex,
    _VertexPos,
    _VertexPosCol,
    _VertexPosTex,
};

struct VertexBuffer {
    VertexBuffer(void* data, uint32 numVertices, VertexType vertexType);

    virtual ~VertexBuffer();

    void bind();

    void unbind();

private:
    GLuint bufferId;
    GLuint vao;
};

