#pragma once
#include "defines.h"

#include <vector>
#include <fstream>
#include <iostream>
#include "libs/stb_image.h"
#include "libs/glm/glm.hpp"

#include "shader.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"



#ifdef _DEBUG123
void _GLGetError(const char* file, int line, const char* call);
#define GLCALL(call) call; _GLGetError(__FILE__, __LINE__, #call)
#else
#define GLCALL(call) call
#endif

struct BMFMaterial {
    glm::vec3 diffuse;
    glm::vec3 specular;
    glm::vec3 emissive;
    float shininess;
};

struct Material {
    BMFMaterial material;
    GLuint diffuseMap;
    GLuint normalMap;
};

class Mesh {
public:
    Mesh(std::vector<Vertex>& vertices, uint64 numVertices, std::vector<uint32>& indices, uint64 numIndices, Material material, Shader* shader);

    ~Mesh();

    glm::vec3 getDimension();

    inline void render();

    inline void renderShadowMap();

    inline void renderEnvMap();

private:

    glm::vec3 calculateDimension(std::vector<Vertex>& vertices);

    VertexBuffer* vertexBuffer;
    IndexBuffer* indexBuffer;
    Shader* shader;
    Material material;
    uint64 numIndices = 0;
    int diffuseLocation;
    int specularLocation;
    int emissiveLocation;
    int shininessLocation;
    int diffuseMapLocation;
    int normalMapLocation;

    glm::vec3 dimension = glm::vec3(0.0f, 0.0f, 0.0f);
};

class Model {
public:
    bool init(const char* filename, Shader* shader);

    void render();

    void renderShadowMap();

    void renderEnvMap();

    glm::vec3 getDimension();

    glm::vec3 getBoundingBoxDimension();

    ~Model();

    void setModelName(std::string newModelName);

    std::string getModelName();

    void setModelID(int newModelID);

    int getModelID();

    bool getHasTransparentTexture()
    {
        return hasTransparentTexture;
    }

private:
    std::vector<Mesh*> meshes;
    std::vector<Material> materials;

    glm::vec3 dimension = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 boundingBoxDimension = glm::vec3(0.0f, 0.0f, 0.0f);

    std::string modelName="";
    int modelID=-1;
    bool hasTransparentTexture = false;
};