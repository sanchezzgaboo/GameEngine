#pragma once
#include "geometry/Mesh.h"
#include "math/Mat4.h"

struct Object
{
    mesh mesh;
    mat4x4 transform;
};
