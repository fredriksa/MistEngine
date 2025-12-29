#include "Engine.h"

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include "imgui-SFML.h"

#include "EngineContext.hpp"
#include "Engine/EngineLoader.h"
#include "Systems/AssetRegistrySystem.h"
#include "Systems/DataAssetRegistrySystem.h"
#include "Systems/InputSystem.h"
#include "Systems/SceneManagerSystem.h"
#include "Systems/WorldObjectSystem.h"
#include "Systems/ImGuiSystem.h"
#include "Systems/CoordinateProjectionSystem.h"
#include "Systems/ShaderPipeline.h"

namespace Core
{
    Engine::Engine()
        : SystemsRegistry(std::make_shared<Core::SystemsRegistry>())
    {
        Context = std::make_shared<EngineContext>();
        Context->Engine = this;
        Context->WindowSize = sf::Vector2u(1920, 1080);
        Context->Window = std::make_shared<sf::RenderWindow>(sf::VideoMode(Context->WindowSize), "Mist Engine");
        Context->Window->requestFocus();
        Context->SystemsRegistry = SystemsRegistry;
        Context->Renderer = Context->Window.get();

        SystemsRegistry->Register<AssetRegistrySystem>(Context);
        SystemsRegistry->Register<ImGuiSystem>(Context);
        SystemsRegistry->Register<InputSystem>(Context);
        SystemsRegistry->Register<SceneManagerSystem>(Context);
        SystemsRegistry->Register<DataAssetRegistrySystem>(Context);
        SystemsRegistry->Register<WorldObjectSystem>(Context);
        SystemsRegistry->Register<CoordinateProjectionSystem>(Context);
        SystemsRegistry->Register<ShaderPipeline>(Context);
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
        Window->setActive(true);
        Window->clear();
        Window->display();

        sf::RenderTexture SceneTexture(Context->WindowSize);
        sf::RenderTexture FinalTexture(Context->WindowSize);

        std::shared_ptr<ShaderPipeline> Pipeline = SystemsRegistry->GetCoreSystem<ShaderPipeline>();

        while (Window->isOpen())
        {
            float DeltaTimeS = FrameClock.restart().asSeconds();

            AccumulatedTime += DeltaTimeS;
            FrameCount++;

            if (AccumulatedTime >= 0.5f)
            {
                AverageFPS = FrameCount / AccumulatedTime;
                Window->setTitle("Mist Engine (" + std::to_string(static_cast<int>(AverageFPS)) + " FPS)");
                AccumulatedTime = 0.0f;
                FrameCount = 0;
            }

            ForEachSystem([&DeltaTimeS](const std::shared_ptr<CoreSystem>& System)
            {
                System->Tick(DeltaTimeS);
            });

            Context->Renderer = &SceneTexture;
            SceneTexture.clear();

            ForEachSystem([](const std::shared_ptr<CoreSystem>& System)
            {
                System->Render();
            });

            SceneTexture.display();

            Pipeline->ApplyAll(SceneTexture, FinalTexture);

            Context->Renderer = Window.get();
            Context->Renderer->clear();
            Context->Renderer->setView(Window->getDefaultView());
            sf::Sprite FinalSprite(FinalTexture.getTexture());
            Context->Renderer->draw(FinalSprite);

            ForEachSystem([](const std::shared_ptr<CoreSystem>& System)
            {
                System->RenderUI();
            });

            ImGui::SFML::Render(*Window);

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
