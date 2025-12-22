#pragma once

#include "CoreSystem.hpp"
#include "../ThirdParty/json.hpp"

namespace Core
{
    struct DataAsset;
    class WorldObject;
    class World;

    class WorldObjectSystem : public CoreSystem
    {
    public:
        static constexpr ECoreSystemType Type = ECoreSystemType::WorldObjectSystem;

        WorldObjectSystem(std::shared_ptr<EngineContext> InContext);
        std::shared_ptr<WorldObject> Create(World* TargetWorld, const DataAsset& DataAsset, const nlohmann::json& OverrideValues);
        std::shared_ptr<WorldObject> Create(World* TargetWorld, const nlohmann::json& ObjectData);
    };
}
