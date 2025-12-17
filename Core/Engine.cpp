#include "Engine.h"

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

#include "EngineContext.hpp"
#include "Engine/EngineLoader.h"
#include "Systems/AssetRegistrySystem.h"
#include "Systems/DataAssetRegistrySystem.h"
#include "Systems/InputSystem.h"
#include "Systems/SceneManagerSystem.h"
#include "Systems/WorldObjectSystem.h"
#include "Systems/ImGuiSystem.h"

namespace Core
{
    Engine::Engine()
        : SystemsRegistry(std::make_shared<Core::SystemsRegistry>())
    {
        Context = std::make_shared<EngineContext>();
        Context->Engine = this;
        Context->WindowSize = sf::Vector2u(1920, 1080);
        Context->Window = std::make_shared<sf::RenderWindow>(sf::VideoMode(Context->WindowSize), "Mist Engine");
        Context->SystemsRegistry = SystemsRegistry;

        SystemsRegistry->Register<AssetRegistrySystem>(Context);
        SystemsRegistry->Register<InputSystem>(Context);
        SystemsRegistry->Register<SceneManagerSystem>(Context);
        SystemsRegistry->Register<DataAssetRegistrySystem>(Context);
        SystemsRegistry->Register<WorldObjectSystem>(Context);
        SystemsRegistry->Register<ImGuiSystem>(Context);
    }

    void Engine::Run()
    {
        setvbuf(stdout, NULL, _IONBF, 0);
        std::printf("Engine::Run()\n");

        ForEachSystem([](const std::shared_ptr<CoreSystem>& System)
        {
            System->Start();
        });

        EngineLoader Loader(Context);
        Loader.LoadGlobalAssets();

        sf::Clock FrameClock;
        FrameClock.start();

        const std::shared_ptr<sf::RenderWindow>& Window = Context->Window;
        while (Window->isOpen())
        {
            float DeltaTimeS = FrameClock.restart().asSeconds();

            ForEachSystem([&DeltaTimeS](const std::shared_ptr<CoreSystem>& System)
            {
                System->Tick(DeltaTimeS);
            });

            Window->clear();

            ForEachSystem([](const std::shared_ptr<CoreSystem>& System)
            {
                System->Render();
            });

            ForEachSystem([](const std::shared_ptr<CoreSystem>& System)
            {
                System->RenderUI();
            });

            Window->display();

            if (bPendingShutdown)
            {
                Window->close();
            }
        }

        ForEachSystem([](const std::shared_ptr<CoreSystem>& System)
        {
            System->Shutdown();
        });

        std::printf("~Engine::Run()\n");
    }

    void Engine::Shutdown()
    {
        bPendingShutdown = true;
    }
}
