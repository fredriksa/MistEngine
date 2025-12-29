#pragma once

#include "Component.h"

namespace Core
{
    class PlayerSpawnComponent : public Component
    {
    public:
        PlayerSpawnComponent(const std::shared_ptr<WorldObject>& Owner, std::shared_ptr<EngineContext> Context);

        void Start() override;
    };
}
