#pragma once

#include "Component.h"
#include <SFML/System/Vector2.hpp>

namespace Core
{
    class TransformComponent : public Component
    {
    public:
        TransformComponent(const std::shared_ptr<WorldObject>& Owner);

        sf::Vector2f Position{0.0f, 0.0f};

        bool Initialize(const nlohmann::json& Data) override;
    };
}
