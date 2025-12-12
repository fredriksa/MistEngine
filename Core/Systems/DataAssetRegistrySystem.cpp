#include "DataAssetRegistrySystem.h"

namespace Core
{
    DataAssetRegistrySystem::DataAssetRegistrySystem(std::shared_ptr<EngineContext> InContext)
        : CoreSystem("DataAssetRegistrySystem", Type, std::move(InContext))
    {
    }

    // :TODO: Maybe we can get rid of this func?
    std::shared_ptr<DataAsset> DataAssetRegistrySystem::Load(const std::string& Path)
    {
        std::optional<DataAsset> Asset = DataAsset::LoadFromFile(Path);
        if (!Asset)
        {
            return nullptr;
        }

        if (Cache.contains(Asset->Name))
        {
            return Cache[Asset->Name];
        }

        auto SharedAsset = std::make_shared<DataAsset>(std::move(Asset.value()));
        Cache[SharedAsset->Name] = SharedAsset;
        std::printf("DataAsset '%s' loaded and cached\n", SharedAsset->Name.c_str());
        return SharedAsset;
    }

    std::shared_ptr<DataAsset> DataAssetRegistrySystem::Get(const std::string& Name)
    {
        if (Cache.contains(Name))
        {
            return Cache[Name];
        }

        std::printf("DataAsset '%s' not found in cache\n", Name.c_str());
        return nullptr;
    }

    void DataAssetRegistrySystem::Store(const std::string& Name, std::shared_ptr<DataAsset> Asset)
    {
        Cache[Name] = Asset;
    } 
}
