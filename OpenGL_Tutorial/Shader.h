#pragma once
#include <GL/glew.h>
#include <string>
#include "defines.h"

struct Shader {
    Shader(std::string vertexShaderFilename, std::string fragmentShaderFilename);
    virtual ~Shader();

    void bind();
    void unbind();

    GLuint getShaderId() {
        return shaderId;
    }

private:

    GLuint compile(std::string shaderSource, GLenum type);
    std::string parse(std::string filename);
    GLuint createShader(std::string vertexShaderFilename, std::string fragmentShaderFilename);

    GLuint shaderId;
};

