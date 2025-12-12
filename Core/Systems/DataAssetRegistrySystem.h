#pragma once

#include "CoreSystem.hpp"
#include "CoreSystems.h"
#include "../Assets/DataAsset.h"

namespace Core
{
    class DataAssetRegistrySystem : public CoreSystem
    {
    public:
        static constexpr ECoreSystemType Type = ECoreSystemType::DataAssetRegistrySystem;

        DataAssetRegistrySystem(std::shared_ptr<EngineContext> InContext);
        ~DataAssetRegistrySystem() override = default;

        std::shared_ptr<DataAsset> Load(const std::string& Path);
        std::shared_ptr<DataAsset> Get(const std::string& Name);
        void Store(const std::string& Name, std::shared_ptr<DataAsset> Asset);

    private:
        std::unordered_map<std::string, std::shared_ptr<DataAsset>> Cache;
    };
}
