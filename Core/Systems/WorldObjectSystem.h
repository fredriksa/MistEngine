#pragma once

#include "CoreSystem.hpp"
#include "../ThirdParty/json.hpp"

namespace Core
{
    struct DataAsset;
    class WorldObject;

    class WorldObjectSystem : public CoreSystem
    {
    public:
        static constexpr ECoreSystemType Type = ECoreSystemType::WorldObjectSystem;

        WorldObjectSystem(std::shared_ptr<EngineContext> InContext);
        std::shared_ptr<WorldObject> Create(const DataAsset& DataAsset, const nlohmann::json& OverrideValues);
    };
}
