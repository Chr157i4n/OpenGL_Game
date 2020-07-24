#pragma once
#include "defines.h"

#include <GL/glew.h>
#include <string>

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

