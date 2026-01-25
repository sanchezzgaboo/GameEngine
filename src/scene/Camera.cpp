#include "Camera.h"

Camera::Camera() : x(1.0f), y(1.0f), z(1.0f), vCamera(0.f,0.f,0.f), vLookDir(0.f,0.f,-1.0f), vUp(0.f,1.f,0.f), vTarget(vCamera + vLookDir)
{
}