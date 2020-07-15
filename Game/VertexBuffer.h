#pragma once

#include "defines.h"
#include <GL/glew.h>
#include <stddef.h>
#include <vector>


struct VertexBuffer {
    VertexBuffer(void* data, uint32 numVertices) {
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        glGenBuffers(1, &bufferId);
        glBindBuffer(GL_ARRAY_BUFFER, bufferId);
        glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(Vertex), data, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(struct Vertex, position));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(struct Vertex, normal));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(struct Vertex, textureCoord));

        glBindVertexArray(0);
    }

    virtual ~VertexBuffer() {
        glDeleteBuffers(1, &bufferId);
    }

    void bind() {
        glBindVertexArray(vao);
    }

    void unbind() {
        glBindVertexArray(0);
    }

private:
    GLuint bufferId;
    GLuint vao;
};

