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
#include "../Tilemap/TileSheet.h"

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

        std::shared_ptr<AssetRegistrySystem> AssetRegistry = Context->SystemsRegistry->GetCoreSystem<AssetRegistrySystem>();
        std::shared_ptr<DataAssetRegistrySystem> DataAssetRegistry = Context->SystemsRegistry->GetCoreSystem<DataAssetRegistrySystem>();
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

        LoadGlobalTileSheets();
    }

    void EngineLoader::LoadGlobalTileSheets()
    {
        const std::string TileSheetDirectory = "Game/Assets/Tilesheets/";
        if (!std::filesystem::exists(TileSheetDirectory))
        {
            std::printf("TileSheet directory does not exist: %s\n", TileSheetDirectory.c_str());
            return;
        }

        std::shared_ptr<AssetRegistrySystem> AssetRegistry = Context->SystemsRegistry->GetCoreSystem<AssetRegistrySystem>();
        std::shared_ptr<DataAssetRegistrySystem> DataAssetRegistry = Context->SystemsRegistry->GetCoreSystem<DataAssetRegistrySystem>();
        AssetLoader Loader(AssetRegistry, DataAssetRegistry);

        std::vector<std::string> TileSheetPaths;

        for (const auto& Entry : std::filesystem::directory_iterator(TileSheetDirectory))
        {
            if (Entry.is_regular_file() && Entry.path().extension() == ".tilesheet")
            {
                std::string Path = Entry.path().string();
                std::replace(Path.begin(), Path.end(), '\\', '/');
                TileSheetPaths.push_back(Path);
                Loader.QueueTexture(Path);
            }
        }

        if (TileSheetPaths.empty())
        {
            std::printf("No tilesheets found in %s\n", TileSheetDirectory.c_str());
            return;
        }

        Task<std::vector<AssetId>> LoadTask = Loader.LoadAllAsync();
        while (!LoadTask.await_ready())
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(3));
        }

        std::printf("Loaded %d tilesheet textures\n", Loader.GetCompletedCount());

        CreateTileSheetObjects();
    }

    void EngineLoader::CreateTileSheetObjects()
    {
        const std::string TileSheetDirectory = "Game/Assets/Tilesheets/";
        std::shared_ptr<AssetRegistrySystem> AssetRegistry = Context->SystemsRegistry->GetCoreSystem<AssetRegistrySystem>();

        uint TileSheetIdCounter = 1;

        for (const auto& Entry : std::filesystem::directory_iterator(TileSheetDirectory))
        {
            if (Entry.is_regular_file() && Entry.path().extension() == ".tilesheet")
            {
                std::string Path = Entry.path().string();
                std::replace(Path.begin(), Path.end(), '\\', '/');

                std::optional<TileSheet> MaybeTileSheet = TileSheet::Create(Path, AssetRegistry.get());
                if (MaybeTileSheet.has_value())
                {
                    TileSheet Sheet = std::move(MaybeTileSheet.value());
                    Sheet.SetId(TileSheetIdCounter++);

                    // Prefix with "tilesheet://" to avoid collision in PathToAssetId map (texture already uses Path)
                    std::string TileSheetKey = "tilesheet://" + Path;
                    AssetRegistry->Store<TileSheet>(std::make_shared<TileSheet>(Sheet), TileSheetKey);
                    std::printf("Created TileSheet: %s (%dx%d tiles)\n",
                        Sheet.GetName().c_str(),
                        Sheet.GetNumColumns(),
                        Sheet.GetNumRows());
                }
            }
        }
    }
}
