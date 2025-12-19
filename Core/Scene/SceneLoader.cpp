#include "SceneLoader.h"

#include <unordered_set>

#include "../EngineContext.hpp"
#include "../World/World.h"
#include "../Assets/AssetLoader.h"
#include "../Assets/AssetManifest.h"
#include "../Assets/DataAsset.h"
#include "../Systems/AssetRegistrySystem.h"
#include "../Systems/DataAssetRegistrySystem.h"
#include "../Systems/WorldObjectSystem.h"
#include "../Utils/StringUtils.h"
#include "../SystemsRegistry.hpp"

namespace Core
{
    SceneLoader::SceneLoader(std::shared_ptr<EngineContext> Context)
        : Context(std::move(Context))
    {
    }

    SceneLoader::~SceneLoader() = default;

    Task<std::vector<AssetId>> SceneLoader::LoadScene(const std::string& SceneName, World& TargetWorld)
    {
        auto AssetRegistry = Context->SystemsRegistry->GetCoreSystem<AssetRegistrySystem>();
        auto DataAssetRegistry = Context->SystemsRegistry->GetCoreSystem<DataAssetRegistrySystem>();
        Loader = std::make_unique<AssetLoader>(AssetRegistry, DataAssetRegistry);

        std::string ManifestPath = "Game/Assets/Scenes/" + ToLowercase(SceneName) + ".json";
        AssetManifest Manifest = AssetManifest::LoadFromFile(ManifestPath, "Game/Assets/Scenes/");

        QueueAssetsFromManifest(Manifest);
        std::vector<AssetId> LoadedAssets = co_await Loader->LoadAllAsync();

        SpawnObjectsIntoWorld(Manifest, TargetWorld);

        co_return LoadedAssets;
    }

    void SceneLoader::QueueAssetsFromManifest(const AssetManifest& Manifest)
    {
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
            if (!ObjectEntry.Type.empty())
            {
                ObjectsToLoad.emplace(ObjectEntry.Type);
            }
        }
        for (const std::string& ObjectType : ObjectsToLoad)
        {
            Loader->QueueObject(ObjectType);
        }
    }

    void SceneLoader::SpawnObjectsIntoWorld(const AssetManifest& Manifest, World& TargetWorld)
    {
        if (Manifest.Objects.empty())
        {
            return;
        }

        auto DataAssetRegistry = Context->SystemsRegistry->GetCoreSystem<DataAssetRegistrySystem>();
        auto WorldObjectSys = Context->SystemsRegistry->GetCoreSystem<WorldObjectSystem>();

        for (const ObjectEntry& Entry : Manifest.Objects)
        {
            if (Entry.Type.empty())
            {
                TargetWorld.Register(WorldObjectSys->Create(Entry.Overrides));
            }
            else if (const std::shared_ptr<DataAsset>& DataAsset = DataAssetRegistry->Get(Entry.Type))
            {
                TargetWorld.Register(WorldObjectSys->Create(*DataAsset, Entry.Overrides));
            }
        }
    }
}
