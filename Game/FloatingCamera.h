#pragma once
#include "defines.h"

#include "FPSCamera.h"

class FloatingCamera : public FPSCamera 
{

public:
    FloatingCamera(float fov, float width, float height);

    void moveUp(float amount);

private:


};

