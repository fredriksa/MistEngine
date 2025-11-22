#include "Engine.h"

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

#include "EngineContext.hpp"
#include "Systems/AssetRegistrySystem.h"
#include "Systems/InputSystem.h"
#include "Systems/SceneManagerSystem.h"

namespace Core
{
    Engine::Engine()
        : SystemsRegistry(std::make_shared<Core::SystemsRegistry>())
    {
        Context = std::make_shared<EngineContext>();
        Context->WindowSize = sf::Vector2u(1920, 1080);
        Context->Window = std::make_shared<sf::RenderWindow>(sf::VideoMode(Context->WindowSize), "Mist Engine");
        Context->SystemsRegistry = SystemsRegistry;

        SystemsRegistry->Register<AssetRegistrySystem>(Context);
        SystemsRegistry->Register<InputSystem>(Context);
        SystemsRegistry->Register<SceneManagerSystem>(Context);
    }

    void Engine::Run()
    {
        StartSystems();

        sf::Clock FrameClock;
        FrameClock.start();

        const std::shared_ptr<sf::RenderWindow>& Window = Context->Window;
        while (Window->isOpen())
        {
            float DeltaTimeS = FrameClock.restart().asSeconds();

            TickSystems(DeltaTimeS);
            Window->clear();
            RenderSystems();
            Window->display();
        }
        ShutdownSystems();
    }

    void Engine::StartSystems()
    {
        ForEachSystem([](const std::shared_ptr<CoreSystem>& CoreSystem)
        {
            CoreSystem->Start();
        });
    }

    void Engine::TickSystems(float DeltaTimeS)
    {
        ForEachSystem([&DeltaTimeS](const std::shared_ptr<CoreSystem>& CoreSystem)
        {
            CoreSystem->Tick(DeltaTimeS);
        });
    }

    void Engine::RenderSystems()
    {
        ForEachSystem([](const std::shared_ptr<CoreSystem>& CoreSystem)
        {
            CoreSystem->Render();
        });
    }

    void Engine::ShutdownSystems()
    {
        ForEachSystem([](const std::shared_ptr<CoreSystem>& CoreSystem)
        {
            CoreSystem->Shutdown();
        });
    }
}
