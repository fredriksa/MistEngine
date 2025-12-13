#include "DataAssetRegistrySystem.h"

namespace Core
{
    DataAssetRegistrySystem::DataAssetRegistrySystem(std::shared_ptr<EngineContext> InContext)
        : CoreSystem("DataAssetRegistrySystem", Type, std::move(InContext))
    {
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
