#include "Scene.h"

#include <cstdio>

#include "../Engine.h"
#include "../Systems/AssetRegistrySystem.h"
#include "../Scene/SceneLoader.h"

namespace Core
{
    Scene::Scene(std::shared_ptr<EngineContext> Context, std::string Name)
        : Name(std::move(Name))
          , Context(std::move(Context))
          , World(this->Context)
    {
    }

    Scene::~Scene()
    {
    }

    void Scene::Tick(float DeltaTimeS)
    {
        World.Tick(DeltaTimeS);
    }

    void Scene::Render()
    {
        World.Render();
    }

    Task<> Scene::Load()
    {
        std::printf("Scene '%s' loading...\n", Name.c_str());

        SceneLoader Loader(Context);
        LoadedAssets = co_await Loader.LoadScene(Name, World);

        std::printf("Scene '%s' loaded %zu assets\n", Name.c_str(), LoadedAssets.size());
        OnLoad();
        co_return;
    }

    void Scene::Enter()
    {
        std::printf("Scene '%s' entering...\n", Name.c_str());
        OnEnter();
    }

    void Scene::Exit()
    {
        std::printf("Scene '%s' exiting...\n", Name.c_str());

        OnExit();

        auto AssetRegistry = Context->SystemsRegistry->GetCoreSystem<AssetRegistrySystem>();
        for (AssetId Id : LoadedAssets)
        {
            AssetRegistry->Release(Id);
        }

        LoadedAssets.clear();

        std::printf("Scene '%s' exited and released assets\n", Name.c_str());
    }
}
