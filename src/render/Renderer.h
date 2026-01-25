#pragma once
#include "IRenderer.h"
#include <SFML/Graphics.hpp>


class Renderer : public IRenderer
{
public:
    Renderer(sf::RenderWindow& window);

    void beginFrame() override;
    void renderScene(const Scene& scene, const Camera& camera, const mat4x4& projMat) override;
    void endFrame() override;

private:
    sf::RenderWindow& window;
};
