#include "AssetRegistrySystem.h"

#include <cstdio>

namespace Core
{
    AssetRegistrySystem::AssetRegistrySystem(const std::shared_ptr<EngineContext>& InContext)
        : CoreSystem("AssetRegistrySystem", Type, InContext)
    {
    }

    void AssetRegistrySystem::Shutdown()
    {
        AssetToTexture.clear();
        AssetToFont.clear();
        AssetToSound.clear();

        Metadata.clear();
        RefCounts.clear();
        PathToAssetId.clear();

        CoreSystem::Shutdown();
    }

    void AssetRegistrySystem::Release(AssetId Id)
    {
        if (!RefCounts.contains(Id))
        {
            std::printf("Warning: Attempted to release non-existent asset ID\n");
            return;
        }

        RefCounts[Id]--;
        if (RefCounts[Id] == 0)
        {
            Unload(Id);
        }
    }

    void AssetRegistrySystem::Unload(AssetId Id)
    {
        if (!Metadata.contains(Id))
        {
            return;
        }

        const AssetMetadata& Meta = Metadata[Id];

        switch (Meta.Type)
        {
        case AssetType::Texture:
            AssetToTexture.erase(Id);
            break;
        case AssetType::Font:
            AssetToFont.erase(Id);
            break;
        case AssetType::Sound:
            AssetToSound.erase(Id);
            break;
        }

        PathToAssetId.erase(Meta.Path);
        Metadata.erase(Id);
        RefCounts.erase(Id);

        std::printf("Unloaded asset: %s\n", Meta.Path.c_str());
    }
}
