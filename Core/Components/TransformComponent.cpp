#include "TransformComponent.h"

#include "ComponentRegistry.h"

namespace Core
{
    REGISTER_COMPONENT(TransformComponent);

    TransformComponent::TransformComponent(const std::shared_ptr<WorldObject>& Owner)
        : Component(Owner)
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
