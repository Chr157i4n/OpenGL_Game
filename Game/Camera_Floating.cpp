#include "Camera_Floating.h"

FloatingCamera::FloatingCamera(float fov, float width, float height) : FPSCamera(fov, width, height) 
{

}

void FloatingCamera::moveUp(float amount) {
    translate(up * amount);
}
