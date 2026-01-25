#pragma once
#include <SFML/Graphics/RenderWindow.hpp>
#include "render/IRenderer.h"
#include "scene/Scene.h"

class Engine
{
public:
    Engine();
    void run();

private:
    sf::RenderWindow window;
    Scene scene;
    std::unique_ptr<IRenderer> renderer;
};
