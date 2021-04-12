#include "Camera_FPS.h"
#include "Game.h"

#include <string>     // std::string, std::stof
#include "ConfigManager.h"

FPSCamera::FPSCamera(float fov, float width, float height) : Camera(fov, width, height) 
{
    up = glm::vec3(0.0f, 1.0f, 0.0f);
    yaw = -90.0f;
    pitch = 0.0f;
    onMouseMoved(0.0f, 0.0f);
    update();
    mouseSensitivity = mouseSensitivity * std::stof(ConfigManager::readConfig("mouse_sensitivity"));
}

void FPSCamera::onMouseMoved(float xRel, float yRel) 
{
    yaw += xRel * mouseSensitivity;
    pitch -= yRel * mouseSensitivity;
    if (pitch > 60)
        pitch = 60;
    if (pitch < -60)
        pitch = -60;

    lookAt_NotNormalized.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
    lookAt_NotNormalized.y = sin(glm::radians(pitch));
    lookAt_NotNormalized.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
    lookAt = glm::normalize(lookAt_NotNormalized);

    //std::cout << "lookAt:   " << lookAt.x << " " << lookAt.y << " " << lookAt.z << std::endl;
    update();
}

void FPSCamera::update()
{
    view = glm::lookAt(cameraposition, cameraposition + lookAt, up);
    viewProj = proj * view;
}

void FPSCamera::moveFront(float amount) 
{
    translate(glm::normalize(glm::vec3(1.0f, 0.0f, 1.0f) * lookAt) * amount);
    update();
}

void FPSCamera::moveSideways(float amount) 
{
    translate(glm::normalize(glm::cross(lookAt, up)) * amount);
    update();
}

void FPSCamera::moveUp(float amount) 
{
    translate(up * amount);
    update();
}
