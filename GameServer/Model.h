#pragma once
#include "defines.h"

#include <vector>
#include <fstream>
#include <iostream>
#include "libs/glm/glm.hpp"

struct BMFMaterial {
    glm::vec3 diffuse;
    glm::vec3 specular;
    glm::vec3 emissive;
    float shininess;
};

struct Material {
    BMFMaterial material;
    int diffuseMap;
    int normalMap;
};

class Mesh {
public:
    Mesh(std::vector<glm::vec3>& vertices, uint64 numVertices, std::vector<uint32>& indices, uint64 numIndices);


    glm::vec3 getDimension();

private:

    glm::vec3 calculateDimension(std::vector<glm::vec3>& vertices);

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
    bool init(const char* filename);

    void render();

    void renderShadowMap();

    void renderEnvMap();

    glm::vec3 getDimension();

    glm::vec3 getBoundingBoxDimension();


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