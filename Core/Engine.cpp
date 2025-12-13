#include "Engine.h"

#include <filesystem>
#include <thread>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

#include "EngineContext.hpp"
#include "Assets/AssetLoader.h"
#include "Assets/AssetManifest.h"
#include "Systems/AssetRegistrySystem.h"
#include "Systems/DataAssetRegistrySystem.h"
#include "Systems/InputSystem.h"
#include "Systems/SceneManagerSystem.h"
#include "Systems/WorldObjectSystem.h"

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
        SystemsRegistry->Register<DataAssetRegistrySystem>(Context);
        SystemsRegistry->Register<WorldObjectSystem>(Context);
    }

    void Engine::LoadGlobalAssets()
    {
        const std::string GlobalAssetPath = "Game/Assets/global.json";
        if (!std::filesystem::exists(GlobalAssetPath))
        {
            std::printf("Could not load global assets with path: %s\n", GlobalAssetPath.c_str());
            return;
        }

        AssetLoader Loader(SystemsRegistry->GetCoreSystem<AssetRegistrySystem>(), SystemsRegistry->GetCoreSystem<DataAssetRegistrySystem>());
        const AssetManifest Manifest = AssetManifest::LoadFromFile(GlobalAssetPath);

        for (const AssetEntry& TextureAssetEntry : Manifest.Textures)
        {
            Loader.QueueTexture(TextureAssetEntry.Path);
        }

        for (const AssetEntry& FontAssetEntry : Manifest.Fonts)
        {
            Loader.QueueFont(FontAssetEntry.Path, FontAssetEntry.Size);
        }

        for (const AssetEntry& SoundAssetEntry : Manifest.Sounds)
        {
            Loader.QueueSound(SoundAssetEntry.Path);
        }

        Task<std::vector<AssetId>> LoadTask = Loader.LoadAllAsync();
        while (!LoadTask.await_ready())
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(3));
        }

        std::printf("Loaded %d global assets\n", Loader.GetCompletedCount());
    }

    void Engine::Run()
    {
        // Disable buffering for printf
        setvbuf(stdout, NULL, _IONBF, 0);
        std::printf("Engine::Run()\n");

        StartSystems();
        LoadGlobalAssets();

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
