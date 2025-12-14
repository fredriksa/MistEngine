#include "EngineLoader.h"

#include <filesystem>
#include <cstdio>
#include <thread>
#include <chrono>

#include "../EngineContext.hpp"
#include "../SystemsRegistry.hpp"
#include "../Assets/AssetLoader.h"
#include "../Assets/AssetManifest.h"
#include "../Systems/AssetRegistrySystem.h"
#include "../Systems/DataAssetRegistrySystem.h"

namespace Core
{
    EngineLoader::EngineLoader(std::shared_ptr<EngineContext> Context)
        : Context(std::move(Context))
    {
    }

    EngineLoader::~EngineLoader() = default;

    void EngineLoader::LoadGlobalAssets()
    {
        const std::string GlobalAssetPath = "Game/Assets/global.json";
        if (!std::filesystem::exists(GlobalAssetPath))
        {
            std::printf("Could not load global assets with path: %s\n", GlobalAssetPath.c_str());
            return;
        }

        auto AssetRegistry = Context->SystemsRegistry->GetCoreSystem<AssetRegistrySystem>();
        auto DataAssetRegistry = Context->SystemsRegistry->GetCoreSystem<DataAssetRegistrySystem>();
        AssetLoader Loader(AssetRegistry, DataAssetRegistry);

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
}
