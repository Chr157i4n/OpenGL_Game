#pragma once
#include "defines.h"

#include "Camera.h"

#include <iostream>

class FPSCamera : public Camera {
public:

    FPSCamera(float fov, float width, float height);

    void onMouseMoved(float xRel, float yRel);

    void update() override;

    void moveFront(float amount);

    void moveSideways(float amount);

    void moveUp(float amount);

protected:
    float yaw;
    float pitch;
    glm::vec3 lookAt_NotNormalized;
    glm::vec3 lookAt;
    float mouseSensitivity = 0.3f;
    glm::vec3 up;
};

