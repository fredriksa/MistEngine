#include "TransformComponent.h"

#include "ComponentRegistry.h"

namespace Core
{
    REGISTER_COMPONENT(TransformComponent);

    TransformComponent::TransformComponent(const std::shared_ptr<WorldObject>& Owner, std::shared_ptr<EngineContext> Context)
        : Component(Owner, std::move(Context), "TransformComponent")
    {
    }

    bool TransformComponent::Initialize(const nlohmann::json& Data)
    {
        if (Data.contains("x"))
        {
            Position.x = Data["x"].get<float>();
        }
        if (Data.contains("y"))
        {
            Position.y = Data["y"].get<float>();
        }
        return true;
    }
}
