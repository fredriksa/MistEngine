#pragma once

#include <memory>
#include <string>

#include "../AssetId.hpp"
#include "../AssetLoader.h"

namespace Core
{
    class AssetRegistrySystem;

    class IAssetTypeHandler
    {
    public:
        virtual ~IAssetTypeHandler() = default;

        virtual AssetLoader::LoadedAsset Load(const AssetLoader::LoadRequest& Request) = 0;
        virtual AssetId Store(std::shared_ptr<void> Data, const std::string& Path,
                             AssetRegistrySystem& Registry) = 0;
        virtual void Unload(AssetId Id, AssetRegistrySystem& Registry) = 0;
    };
}
