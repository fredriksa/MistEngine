#pragma once

#include "IAssetTypeHandler.h"

namespace Core
{
    class TextureHandler : public IAssetTypeHandler
    {
    public:
        AssetLoader::LoadedAsset Load(const AssetLoader::LoadRequest& Request) override;
        AssetId Store(std::shared_ptr<void> Data, const std::string& Path,
                     AssetRegistrySystem& Registry) override;
        void Unload(AssetId Id, AssetRegistrySystem& Registry) override;
    };
}
