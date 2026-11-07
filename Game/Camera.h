#pragma once
#include "defines.h"

#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
public:

    Camera(float fov, float width, float height);

    glm::mat4 getViewProj();

    glm::mat4 getProj();

    glm::mat4 getView();

    virtual void update();

    virtual void translate(glm::vec3 v);

protected:
    glm::vec3 cameraposition;
    glm::mat4 view;
    glm::mat4 proj;
    glm::mat4 viewProj;
};

