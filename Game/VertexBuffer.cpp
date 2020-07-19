#include "VertexBuffer.h"

VertexBuffer::VertexBuffer(void* data, uint32 numVertices, VertexType vertexType) {
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &bufferId);
    glBindBuffer(GL_ARRAY_BUFFER, bufferId);

    if (vertexType == VertexType::_VertexPos)
    {
        glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(VertexPos), data, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPos), (void*)offsetof(struct VertexPos, position));
    }
    if (vertexType == VertexType::_VertexPosCol)
    {
        glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(VertexPosCol), data, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPosCol), (void*)offsetof(struct VertexPosCol, position));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(VertexPosCol), (void*)offsetof(struct VertexPosCol, color));
    }
    if (vertexType == VertexType::_VertexPosTex)
    {
        glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(VertexPosTex), data, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPosTex), (void*)offsetof(struct VertexPosTex, position));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VertexPosTex), (void*)offsetof(struct VertexPosTex, textureCoord));
    }
    if (vertexType == VertexType::_Vertex)
    {
        glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(Vertex), data, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(struct Vertex, position));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(struct Vertex, normal));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(struct Vertex, tangent));
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(struct Vertex, textureCoord));
    }

    glBindVertexArray(0);
}

VertexBuffer::~VertexBuffer() {
    glDeleteBuffers(1, &bufferId);
}

void VertexBuffer::bind() {
    glBindVertexArray(vao);
}

void VertexBuffer::unbind() {
    glBindVertexArray(0);
}