#pragma once
#include <vector>
#include "Object.h"

struct Scene
{
    std::vector<Object> objects;
    Camera camera;
};
