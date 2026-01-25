#pragma once
#include "Object.h"
#include "Scene.h"
#include "Camera.h"

class IRenderer{
    public:
        virtual ~IRenderer() = default;
        virtual void beginFrame() = 0;
        virtual void renderScene(const Scene& scene, const Camera& camera, const mat4x4& projMat) = 0;
        virtual void endFrame() = 0;
};