#include "Scene.h"

#include <cstdio>

#include "Engine.h"
#include "Systems/AssetRegistrySystem.h"
#include "Systems/DataAssetRegistrySystem.h"
#include "Assets/AssetManifest.h"
#include "Assets/AssetLoader.h"
#include "Systems/WorldObjectSystem.h"
#include "Utils/StringUtils.h"

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

        std::shared_ptr<AssetRegistrySystem> AssetRegistry = Context->SystemsRegistry->GetCoreSystem<
            AssetRegistrySystem>();
        std::shared_ptr<DataAssetRegistrySystem> DataAssetRegistry = Context->SystemsRegistry->GetCoreSystem<
            DataAssetRegistrySystem>();
        Loader = std::make_unique<AssetLoader>(AssetRegistry, DataAssetRegistry);

        std::string ManifestPath = "Game/Assets/Scenes/" + ToLowercase(Name) + ".json";

        AssetManifest Manifest = AssetManifest::LoadFromFile(ManifestPath, "Game/Assets/Scenes/");

        for (const auto& TextureEntry : Manifest.Textures)
        {
            Loader->QueueTexture(TextureEntry.Path);
        }
        for (const auto& FontEntry : Manifest.Fonts)
        {
            Loader->QueueFont(FontEntry.Path, FontEntry.Size);
        }
        for (const auto& SoundEntry : Manifest.Sounds)
        {
            Loader->QueueSound(SoundEntry.Path);
        }

        std::unordered_set<std::string> ObjectsToLoad;
        for (const auto& ObjectEntry : Manifest.Objects)
        {
            ObjectsToLoad.emplace(ObjectEntry.Type);
        }
        for (const std::string& ObjectType : ObjectsToLoad)
        {
            Loader->QueueObject(ObjectType);
        }

        LoadedAssets = co_await Loader->LoadAllAsync();

        std::printf("Scene '%s' loaded %zu assets\n", Name.c_str(), LoadedAssets.size());

        if (!Manifest.Objects.empty())
        {
            std::shared_ptr<WorldObjectSystem> WorldObjectSys = Context->SystemsRegistry->GetCoreSystem<
                WorldObjectSystem>();
            for (const ObjectEntry& Entry : Manifest.Objects)
            {
                if (const std::shared_ptr<DataAsset>& DataAsset = DataAssetRegistry->Get(Entry.Type))
                {
                    World.Register(WorldObjectSys->Create(*DataAsset, Entry.Overrides));
                }
            }
        }

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
