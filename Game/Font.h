#pragma once
#include "defines.h"

#include "libs/stb_truetype.h"
#include <cstdio>
#include <GL/glew.h>

#include "Shader.h"

#ifdef _DEBUG123
void _GLGetError(const char* file, int line, const char* call);
#define GLCALL(call) call; _GLGetError(__FILE__, __LINE__, #call)
#else
#define GLCALL(call) call
#endif

struct FontVertex {
    glm::vec2 position;
    glm::vec2 texCoords;
};

struct Font {
    ~Font();

    void initFont(const char* filename);

    void drawString(float x, float y, const char* text, Shader* fontShader);

private:
    stbtt_bakedchar cdata[96];
    GLuint fontTexture;
    GLuint fontVao;
    GLuint fontVertexBufferId;
    FontVertex* fontVertexBufferData = 0;
    uint32 fontVertexBufferCapacity;
};

