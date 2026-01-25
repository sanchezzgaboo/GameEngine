#include "Engine.h"
#include "render/Renderer.h"
#include <imgui.h>
#include <imgui-SFML.h>

Engine::Engine()
: window(sf::VideoMode({800, 600}), "Engine")
{
    renderer = std::make_unique<Renderer>(window);
}

void Engine::run()
{
    sf::Clock clock;
    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            // Pass the polled event to ImGui properly (one event at a time)
            //ImGui::SFML::ProcessEvent(window, *event);
            if (event->is<sf::Event::Closed>()){
                window.close();
            }

            //events go here
        }
        sf::Time elapsedTime = clock.restart();

        ImGui::SFML::Update(window, elapsedTime);
        renderer->beginFrame();
        renderer->renderScene(scene, scene.camera);
        // ----- ImGui starts here -----
        ImGui::Begin("Debug");
        ImGui::End();
        // ----- ImGui ends here -----

        ImGui::SFML::Render(window);
        renderer->endFrame();
    }
}
