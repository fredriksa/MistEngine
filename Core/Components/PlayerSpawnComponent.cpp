#include "PlayerSpawnComponent.h"
#include "ComponentRegistry.h"

namespace Core
{
    REGISTER_COMPONENT(PlayerSpawnComponent);

    PlayerSpawnComponent::PlayerSpawnComponent(const std::shared_ptr<WorldObject>& Owner,
                                               std::shared_ptr<EngineContext> Context)
        : Component(Owner, std::move(Context), "PlayerSpawnComponent")
    {
    }

    void PlayerSpawnComponent::Start()
    {
    }
}
